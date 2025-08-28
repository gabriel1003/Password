# Versão mínima do CMake
cmake_minimum_required(VERSION 3.16)

# Projeto
project(GeradorDeSenha VERSION 1.0 LANGUAGES CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6: Core e Sql (QtSql)
find_package(Qt6 COMPONENTS Core Sql REQUIRED)

# Necessário por causa de classes com Q_OBJECT
set(CMAKE_AUTOMOC ON)

# Executável e fontes
add_executable(gerador_de_senha
    src/main.cpp
    src/password_db/models/model.cpp
    src/password_db/repositories/repository.cpp
    src/password_db/repositories/user_repository.cpp
    src/password_db/services/password_service.cpp
    src/password_db/services/crypt_service.cpp
    src/password_db/services/auth_service.cpp
    src/generate_password/generator_password.cpp
    src/qmvc/password_list_model.cpp
    src/qmvc/controller.cpp
    main.qml
    ui/welcome.qml
    ui/show_options.qml
    ui/login.qml
    ui/created_login.qml
)

# Includes do projeto
target_include_directories(gerador_de_senha PRIVATE
    src
    main.qml
    ui
)

# Link com os módulos do Qt
target_link_libraries(gerador_de_senha PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::qml
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    OpenSSL::Crypto
)