#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char *auth = NULL;
char *service = NULL;

int main(void) {
    char commande_saisie[132]; 

    while (1) {
        printf("%p, %p \n", (void*)auth, (void*)service);

        if (fgets(commande_saisie, 0x80, stdin) == NULL) {
            return 0; // Quitte si erreur ou Fin de Fichier (Ctrl+D)
        }

        if (strncmp(commande_saisie, "auth ", 5) == 0) {
            
            auth = malloc(4); 
            if (auth == NULL) return 1;
            
            memset(auth, 0, 4);

            char *argument = &commande_saisie[5];

            size_t longueur = strlen(argument);

            if (longueur < 0x1f) {
                strcpy(auth, argument);
            }
        }
    }

    return 0;
}