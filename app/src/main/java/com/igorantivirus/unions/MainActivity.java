package com.igorantivirus.unions;

import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {
    // Название вашей so-библиотеки (без 'lib' и '.so')
    @Override protected String[] getLibraries() {
        return new String[] { "Unions" };
    }
}
