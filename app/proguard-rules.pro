# Сохраняем все классы и методы SDL, чтобы JNI мог их найти
-keep class org.libsdl.app.** { *; }

# Если вы используете RmlUi или другие библиотеки, которые вызывают Java из C++, 
# добавьте также их пакеты. Например:
-keep class com.igorantivirus.unions.** { *; }

# Не позволять переименовывать нативные методы
-keepclasseswithmembernames class * {
    native <methods>;
}
