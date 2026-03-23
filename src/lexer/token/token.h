#include <QString>
enum class TokenType {
    Type,
    Id,

    Unknown
};
struct Token
{
    TokenType token;
    int line;
    int column
};
