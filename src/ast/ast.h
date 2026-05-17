#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QPair>

#include "src/lexer/token/token.h"

enum class AstNodeKind : int
{
    Program          = 0,
    FunctionProto    = 1,
    Param            = 2,
    Type             = 3,
    Identifier       = 4
};

struct AstParam
{
    QString typeName;
    QString name;
    int     typeLine     = 0;
    int     typeLetterPos = 0;
    int     nameLine     = 0;
    int     nameLetterPos = 0;
    bool    typeValid    = true;
    bool    nameValid    = true;
};

struct AstFunction
{
    QString returnType;
    QString name;
    QVector<AstParam> params;
    int     returnTypeLine     = 0;
    int     returnTypeLetterPos = 0;
    int     nameLine           = 0;
    int     nameLetterPos      = 0;
    bool    returnTypeValid    = true;
    bool    nameValid          = true;
};

struct AstProgram
{
    QVector<AstFunction> functions;
};

struct SemanticError
{
    int     rule;
    int     line;
    int     letterPos;
    QString message;

    SemanticError(int r = 0, int ln = 0, int lp = 0, QString msg = QString())
        : rule(r), line(ln), letterPos(lp), message(std::move(msg)) {}
};

class AST : public QObject
{
    Q_OBJECT
public:
    explicit AST(QObject* parent = nullptr);

    Q_INVOKABLE QString buildTree(const QString& text);

signals:
    void semanticErrorsReady(const QVector<SemanticError>& errors);
    void astTextReady(const QString& tree);

private:
    AstProgram buildFromTokens(const QVector<Token>& tokens);
    QVector<SemanticError> analyze(const AstProgram& program);

    QString renderTree(const AstProgram& program) const;
    QString renderErrors(const QVector<SemanticError>& errors) const;
    QString renderApplicabilityNote() const;

    static bool isSupportedType(const QString& name);
    static bool isValueType(const QString& name);
};
