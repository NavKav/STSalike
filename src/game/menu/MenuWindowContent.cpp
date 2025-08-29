//
// Created by BDCMarley on 06/05/2025.
//

#include "MenuWindowContent.h"

using namespace std;

/*
 * La dernière fois on avait vu que le programme créait un "User", responsable des inputs du joueur, et que ce user
 * appelait en boucle la méthode process de MenuWindowContent. Chaque appel à process est une frame du jeu.
 */
void MenuWindowContent::process()
{
    /*************************************************************************/
    /*************************  GESTION INPUTS  ******************************/
    /*************************************************************************/

    // La variable user est une variable globale du code. Elle possède des infos sur les actions du joueur :

    user[USER_KEY_LEFT];         // renvoie une structure d'information sur la touche "Flèche GAUCHE"
    user[USER_KEY_LEFT].pressed; // booléen, touche appuyée ou non
    user[USER_KEY_LEFT].u;       // long int, temps en ms auquel la touche a été appuyée
    user[USER_KEY_LEFT].v;       // long int, temps en ms auquel la touche a été relachée
                                 // notamment user[USER_KEY_LEFT].u - user[USER_KEY_LEFT].v donne le temps total
                                 // pendant lequel la touche a été appuyée

    /*************************************************************************/
    /*************************  GESTION AFFICHAGE  ***************************/
    /*************************************************************************/

    // La variable window est la seconde variable globa du code. Elle possède des méthodes pour afficher des images dans
    // la fenêtre :

    window.getX();
    window.getY(); // renvoie respectivement la longueur et la hauteur de la
                   // fenêtre.
                   // L'axe des x part de l'angle haut gauche vers l'angle
                   // haut droit. L'axe des y part de l'angle haut gauche vers
                   // l'angle bas gauche :
                   // (0,0) +--------------------------------> +X (droite)
                   //       |
                   //       |
                   //       |
                   //       |
                   //       |
                   //       |
                   //       V
                   //       +Y (bas)

    window.drawIMG(200, 300, "image/map.png"); // dessine l'image "fond.bmp" à la coordonnée (200;300) de
                                               // la fenêtre (en commencant par le pixel de l'image le plus
                                               // en haut et à gauche en (200;300) sur la fenêtre.
                                               // La fonction va chercher le chemin en commençant par
                                               // "ressource/" qui se trouve dans le dossier de l'executable
                                               // (i.e. le dossier "cmake-build-debug")

    /*************************************************************************/
    /*************************  EXEMPLE  *************************************/
    /*************************************************************************/

    // Personnage qui se déplace sur une map

    static int paladinX = window.getX() / 2;
    static int paladinY = window.getY() / 2;

    window.drawIMG(0, 0, "image/menu/map.png");

    if (user[USER_KEY_LEFT].pressed)
        paladinX -= 50;
    if (user[USER_KEY_RIGHT].pressed)
        paladinX += 50;
    if (user[USER_KEY_UP].pressed)
        paladinY -= 50;
    if (user[USER_KEY_DOWN].pressed)
        paladinY += 50;

    window.drawIMG(paladinX, paladinY, "image/menu/paladin.png");

    window.refresh(); // une fois que l'on a dessiné des images sur la fenêtre, on
                      // rafraîchit celle-ci pour les voir.
}