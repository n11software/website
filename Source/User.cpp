#include <User.hpp>
#include <Link.hpp>
#include <regex>
#include <iostream>
#include <json.hpp>
#include <API.hpp>

Error CreateUser(User& user) {
    // Validate user info
    if (user.Email.empty()) return Error{"Email is required!", 1};
    if (user.Password.empty()) return Error{"Password is required!", 2};
    
    // Check if email is valid
    if (!std::regex_match(user.Email, std::regex("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$"))) return Error{"Invalid email!", 3};

    // Check if email is already in use
    if (!sql("SELECT * FROM User WHERE Email='"+user.Email+"';").empty()) return Error{"Email is already in use!", 4};

    // Validate password
    if (user.Password.length() < 8) return Error{"Password must be at least 8 characters!", 5};
    if (!std::regex_match(user.Password, std::regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]).*$"))) return Error{"Password must contain at least one lowercase letter, one uppercase letter, one number, and one special character!", 6};
    // replace all ' with \'
    user.Password = std::regex_replace(user.Password, std::regex("'"), "\\'");

    // Create user
    nlohmann::json SQLRet = sql("CREATE User SET Email='"+user.Email+"', Password=crypto::argon2::generate('"+user.Password+"'), MFA=False, SMS=False, PhoneNumbers=[], Emails=[], Authenticator=False, Created=time::now()");
    if (SQLRet.empty()) return Error{"Failed to create user!", -1};

    std::cout << SQLRet[0] << std::endl;
    user.Authenticator = SQLRet[0]["Authenticator"];
    user.Created = SQLRet[0]["Created"];
    user.Email = SQLRet[0]["Email"];
    user.Emails = SQLRet[0]["Emails"];
    user.MFA = SQLRet[0]["MFA"];
    user.Password = SQLRet[0]["Password"];
    user.PhoneNumbers = SQLRet[0]["PhoneNumbers"];
    user.SMS = SQLRet[0]["SMS"];
    user.ID = SQLRet[0]["id"];

    return Error{"Success", 0};
}