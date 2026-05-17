#include "src/ast/ast.h"
#include "src/lexer/lexser.h"

#include <QSet>
#include <QHash>

namespace
{
    inline void skipSpaces(const QVector<Token>& tokens, int& i)
    {
        while (i < tokens.size() && tokens[i].type == TokenType::Space)
            ++i;
    }

    inline bool isLineEnd(TokenType t)
    {
        return t == TokenType::Semicolon;
    }
}

AST::AST(QObject* parent) : QObject(parent) {}

bool AST::isSupportedType(const QString& name)
{
    static const QSet<QString> kTypes = { "void", "int", "float", "double", "short" };
    return kTypes.contains(name);
}

bool AST::isValueType(const QString& name)
{
    return name == "int" || name == "float" || name == "double" || name == "short";
}

AstProgram AST::buildFromTokens(const QVector<Token>& tokens)
{
    AstProgram program;
    int i = 0;

    while (i < tokens.size())
    {
        skipSpaces(tokens, i);
        if (i >= tokens.size()) break;

        if (tokens[i].type == TokenType::Semicolon)
        {
            ++i;
            continue;
        }

        AstFunction func;

        if (tokens[i].type == TokenType::Type || tokens[i].type == TokenType::InvalidType)
        {
            func.returnType         = tokens[i].lexem;
            func.returnTypeLine     = tokens[i].line;
            func.returnTypeLetterPos = tokens[i].letterPos;
            func.returnTypeValid    = (tokens[i].type == TokenType::Type);
            ++i;
        }
        else
        {
            while (i < tokens.size() && !isLineEnd(tokens[i].type)) ++i;
            if (i < tokens.size()) ++i;
            continue;
        }

        skipSpaces(tokens, i);

        if (i < tokens.size() && tokens[i].type == TokenType::Id)
        {
            func.name         = tokens[i].lexem;
            func.nameLine     = tokens[i].line;
            func.nameLetterPos = tokens[i].letterPos;
            ++i;
        }
        else if (i < tokens.size() && tokens[i].type == TokenType::Unknown)
        {
            func.name         = tokens[i].lexem;
            func.nameLine     = tokens[i].line;
            func.nameLetterPos = tokens[i].letterPos;
            func.nameValid    = false;
            ++i;
        }

        skipSpaces(tokens, i);

        if (i >= tokens.size() || tokens[i].type != TokenType::LParen)
        {
            while (i < tokens.size() && !isLineEnd(tokens[i].type)) ++i;
            if (i < tokens.size()) ++i;
            program.functions.push_back(func);
            continue;
        }
        ++i;

        bool paramsClosed = false;
        while (i < tokens.size())
        {
            skipSpaces(tokens, i);
            if (i >= tokens.size()) break;

            if (tokens[i].type == TokenType::RParen)
            {
                ++i;
                paramsClosed = true;
                break;
            }

            if (tokens[i].type == TokenType::Semicolon)
                break;

            AstParam p;

            if (tokens[i].type == TokenType::Type || tokens[i].type == TokenType::InvalidType)
            {
                p.typeName         = tokens[i].lexem;
                p.typeLine         = tokens[i].line;
                p.typeLetterPos    = tokens[i].letterPos;
                p.typeValid        = (tokens[i].type == TokenType::Type);
                ++i;
            }
            else
            {
                while (i < tokens.size()
                       && tokens[i].type != TokenType::Comma
                       && tokens[i].type != TokenType::RParen
                       && tokens[i].type != TokenType::Semicolon)
                {
                    ++i;
                }
                if (i < tokens.size() && tokens[i].type == TokenType::Comma) { ++i; continue; }
                continue;
            }

            skipSpaces(tokens, i);

            if (i < tokens.size() && tokens[i].type == TokenType::Id)
            {
                p.name         = tokens[i].lexem;
                p.nameLine     = tokens[i].line;
                p.nameLetterPos = tokens[i].letterPos;
                ++i;
            }
            else if (i < tokens.size() && tokens[i].type == TokenType::Unknown)
            {
                p.name         = tokens[i].lexem;
                p.nameLine     = tokens[i].line;
                p.nameLetterPos = tokens[i].letterPos;
                p.nameValid    = false;
                ++i;
            }

            func.params.push_back(p);

            skipSpaces(tokens, i);
            if (i < tokens.size() && tokens[i].type == TokenType::Comma)
            {
                ++i;
                continue;
            }
            if (i < tokens.size() && tokens[i].type == TokenType::RParen)
            {
                ++i;
                paramsClosed = true;
                break;
            }
            if (i < tokens.size() && tokens[i].type == TokenType::Semicolon)
                break;
        }

        Q_UNUSED(paramsClosed);

        skipSpaces(tokens, i);
        if (i < tokens.size() && tokens[i].type == TokenType::Semicolon)
            ++i;

        program.functions.push_back(func);
    }

    return program;
}

QVector<SemanticError> AST::analyze(const AstProgram& program)
{
    QVector<SemanticError> errors;
    QHash<QString, QPair<int, int>> declaredFunctions;

    for (const AstFunction& func : program.functions)
    {
        if (!func.name.isEmpty() && func.nameValid)
        {
            if (declaredFunctions.contains(func.name))
            {
                const auto first = declaredFunctions.value(func.name);
                errors.emplace_back(
                    1, func.nameLine, func.nameLetterPos,
                    QString("Повторное объявление функции '%1' (ранее объявлена в %2:%3)")
                        .arg(func.name)
                        .arg(first.first)
                        .arg(first.second)
                );
            }
            else
            {
                declaredFunctions.insert(func.name, qMakePair(func.nameLine, func.nameLetterPos));
            }
        }

        if (!func.returnType.isEmpty() && !func.returnTypeValid)
        {
            errors.emplace_back(
                2, func.returnTypeLine, func.returnTypeLetterPos,
                QString("Тип возвращаемого значения '%1' не поддерживается").arg(func.returnType)
            );
        }

        QHash<QString, QPair<int, int>> paramNames;
        for (const AstParam& p : func.params)
        {
            if (!p.typeName.isEmpty() && !p.typeValid)
            {
                errors.emplace_back(
                    2, p.typeLine, p.typeLetterPos,
                    QString("Тип параметра '%1' не поддерживается").arg(p.typeName)
                );
            }
            else if (p.typeName == "void")
            {
                errors.emplace_back(
                    2, p.typeLine, p.typeLetterPos,
                    QString("Тип 'void' недопустим в качестве типа параметра")
                );
            }

            if (!p.name.isEmpty() && p.nameValid)
            {
                if (paramNames.contains(p.name))
                {
                    const auto first = paramNames.value(p.name);
                    errors.emplace_back(
                        1, p.nameLine, p.nameLetterPos,
                        QString("Повторное имя параметра '%1' в функции '%2' (ранее в %3:%4)")
                            .arg(p.name)
                            .arg(func.name.isEmpty() ? QStringLiteral("<без имени>") : func.name)
                            .arg(first.first)
                            .arg(first.second)
                    );
                }
                else
                {
                    paramNames.insert(p.name, qMakePair(p.nameLine, p.nameLetterPos));
                }
            }
        }
    }

    return errors;
}

QString AST::renderTree(const AstProgram& program) const
{
    QString out;
    out += "Program\n";

    const int total = program.functions.size();
    for (int fi = 0; fi < total; ++fi)
    {
        const AstFunction& f = program.functions[fi];
        const bool lastFunc  = (fi == total - 1);
        const QString fBr    = lastFunc ? "└── " : "├── ";
        const QString fInd   = lastFunc ? "    " : "│   ";

        out += fBr + QString("FunctionPrototype '%1' (стр. %2:%3)\n")
                         .arg(f.name.isEmpty() ? QStringLiteral("<?>") : f.name)
                         .arg(f.nameLine)
                         .arg(f.nameLetterPos);

        const bool hasParams = !f.params.isEmpty();
        out += fInd + "├── ReturnType: "
                   + (f.returnType.isEmpty() ? QStringLiteral("<?>") : f.returnType)
                   + QString(" (стр. %1:%2)").arg(f.returnTypeLine).arg(f.returnTypeLetterPos)
                   + (f.returnTypeValid ? "\n" : "  [неподдерживаемый тип]\n");

        if (!hasParams)
        {
            out += fInd + "└── Params: <none>\n";
            continue;
        }

        out += fInd + "└── Params\n";
        const QString pInd = fInd + "    ";
        const int pTotal   = f.params.size();
        for (int pi = 0; pi < pTotal; ++pi)
        {
            const AstParam& p = f.params[pi];
            const bool lastP  = (pi == pTotal - 1);
            const QString pBr = lastP ? "└── " : "├── ";
            const QString pSub = lastP ? "    " : "│   ";

            out += pInd + pBr + QString("Param '%1' (стр. %2:%3)\n")
                                    .arg(p.name.isEmpty() ? QStringLiteral("<?>") : p.name)
                                    .arg(p.nameLine)
                                    .arg(p.nameLetterPos);
            out += pInd + pSub + "└── Type: "
                       + (p.typeName.isEmpty() ? QStringLiteral("<?>") : p.typeName)
                       + QString(" (стр. %1:%2)").arg(p.typeLine).arg(p.typeLetterPos)
                       + (p.typeValid ? "\n" : "  [неподдерживаемый тип]\n");
        }
    }

    if (program.functions.isEmpty())
        out += "└── (нет прототипов)\n";

    return out;
}

QString AST::renderErrors(const QVector<SemanticError>& errors) const
{
    if (errors.isEmpty())
        return QStringLiteral("Семантических ошибок не обнаружено.\n");

    QString out = QString("Найдено семантических ошибок: %1\n").arg(errors.size());
    for (const SemanticError& e : errors)
    {
        out += QString("[Правило %1] стр. %2:%3 — %4\n")
                   .arg(e.rule)
                   .arg(e.line)
                   .arg(e.letterPos)
                   .arg(e.message);
    }
    return out;
}

QString AST::buildTree(const QString& text)
{
    Lexer lexer;
    const QVector<Token> tokens = lexer.scanReturns(text).first;

    const AstProgram program       = buildFromTokens(tokens);
    const QVector<SemanticError> errs = analyze(program);

    emit semanticErrorsReady(errs);

    QString out;
    out += "=== AST (дерево) ===\n";
    out += renderTree(program);
    out += "\n=== Семантический анализ ===\n";
    out += renderErrors(errs);

    emit astTextReady(out);
    return out;
}

