cmake_minimum_required(VERSION 3.9)
project(Challenge_moteur_jeu)

set(CMAKE_CXX_STANDARD 11)

add_executable(Challenge_moteur_jeu
        avalam-web/example/main.c
        avalam-web/include/avalam.h
        avalam-web/include/topologie.h
        avalam-web/lib/libavalam.a
        avalam-web/lib/libavalam.so
        avalam-web/utils/libavalam.c
        avalam-web/makefile
        lib-avalam/avalam
        lib-avalam/avalam-diag.html
        lib-avalam/avalam-refresh.html
        lib-avalam/avalam-standalone.html
        lib-avalam/avalam.json
        lib-avalam/avalam_vide.png
        lib-avalam/data.json
        lib-avalam/initial.json
        lib-avalam/jaune.png
        lib-avalam/jaune2.png
        lib-avalam/jquery.js
        lib-avalam/rouge.png
        lib-avalam/rouge2.png
        "livrable 1/standalone/standalone.c"
        "livrable 1/standalone/standalone.h")
