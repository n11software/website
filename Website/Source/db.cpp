#include <db.hpp>
#include <fstream>

sql::Driver* driver;
sql::Connection* db;

void connect() {
  std::string db_user, db_pass, db_host, line;
  std::ifstream db_file("db.txt");
  while (std::getline(db_file, line)) {
    if (line.find("user=") != std::string::npos) {
      db_user = line.substr(line.find("user=")+5);
    } else if (line.find("pass=") != std::string::npos) {
      db_pass = line.substr(line.find("pass=")+5);
    } else if (line.find("host=") != std::string::npos) {
      db_host = line.substr(line.find("host=")+5);
    }
  }
  try {
    driver = get_driver_instance();
    db = driver->connect("tcp://" + db_host + ":3306", db_user, db_pass);
    db->setSchema("n11");
  } catch (sql::SQLException &e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
}

sql::Driver* getDriver() {
  return driver;
}

sql::Connection* getConnection() {
  return db;
}