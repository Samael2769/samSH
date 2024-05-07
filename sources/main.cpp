/*
** EPITECH PROJECT, 2022
** Untitled (Workspace)
** File description:
** main
*/

#include <iostream>
#include "macro.hpp"

samsh* sh;

int main()
{
    sh = new samsh();
    return sh->run();
}