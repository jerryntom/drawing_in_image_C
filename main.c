#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_utils.h"

int main(void) {
    char *main_filename = malloc(40 * sizeof(char));
    if(main_filename == NULL) {
        printf("Failed to allocate memory");
        return 8;
    }

    int c;

    printf("Please input main_filename:");
    scanf("%39s", main_filename);
    while((c = getchar()) && c != '\n' && c != EOF) {}

    int err_code = 0;
    struct image_t *main_image = load_image_t(main_filename, &err_code);

    if(main_image == NULL) {
        if(err_code == 2) {
            printf("Couldn't open file");
            free(main_filename);
            return 4;
        }
        else if(err_code == 3) {
            printf("File corrupted");
            free(main_filename);
            return 6;
        }
        else if(err_code == 4) {
            printf("Failed to allocate memory");
            free(main_filename);
            return 8;
        }
    }
    else {
        int num_subimages, counter_input_subimages = 0;

        printf("Please input num of subimages:");

        if(scanf("%d", &num_subimages) != 1) {
            free(main_filename);
            destroy_image(&main_image);
            printf("Incorrect input");
            return 1;
        }
        else if(num_subimages <= 0) {
            free(main_filename);
            destroy_image(&main_image);
            printf("Incorrect input data");
            return 2;
        }

        char *filename = malloc(40 * sizeof(char));
        if(filename == NULL) {
            free(main_filename);
            printf("Failed to allocate memory");
            return 8;
        }

        struct image_t **subimages = malloc(num_subimages * sizeof(struct image_t *));
        int i;

        while(1) {
            printf("Please input a main_filename with subimage:");
            scanf("%39s", filename);
            while((c = getchar()) && c != '\n' && c != EOF) {}

            *(subimages + counter_input_subimages) = load_image_t(filename, &err_code);

            if(*(subimages + counter_input_subimages) == NULL) {
                if(err_code == 2) printf("Couldn't open file\n");
                else if(err_code == 3) printf("File corrupted\n");
                else if(err_code == 4) {
                    printf("Failed to allocate memory");
                    free(main_filename);
                    free(filename);
                    for(i = 0; i < counter_input_subimages; i++) {
                        destroy_image(&(*(subimages + i)));
                    }
                    free(subimages);
                    return 8;
                }
            }
            else {
                int x, y;

                printf("Please input cordinates:");
                if(scanf("%d %d", &x, &y) != 2) {
                    printf("Incorrect input");
                    for(i = 0; i < num_subimages; i++) {
                        if(*(subimages + i) != NULL) destroy_image(&(*(subimages + i)));
                    }
                    free(subimages);
                    subimages = NULL;
                    destroy_image(&main_image);
                    main_image = NULL;
                    free(main_filename);
                    main_filename = NULL;
                    free(filename);
                    filename = NULL;
                    return 1;
                }

                int res;
                res = draw_image(main_image, *(subimages + counter_input_subimages), x, y);
                if(res == 1) printf("Incorrect input data\n");
            }
            counter_input_subimages++;
            if(counter_input_subimages == num_subimages) break;
        }

        char *filename_part = "_modified";
        char *ext = strchr(filename, '.');
        char *ext_part = malloc(40 * sizeof(char));
        strcpy(ext_part, ext);
        int j, dots_counter = 0, temp_dots_counter = 0, last_dot_index = 0;

        for(i = 0; *(main_filename + i) != '\0'; i++) {
            if(*(main_filename + i) == '.') dots_counter++;
        }

        for(i = 0; *(main_filename + i) != '\0'; i++) {
            if(*(main_filename + i) == '.') temp_dots_counter++;
            if(temp_dots_counter == dots_counter) {
                last_dot_index = i;
                break;
            }
        }


        i = last_dot_index;

        while(1){
            if(*(main_filename + i) == '.' || *(main_filename + i) == '\0') {
                for(j = 0; *(filename_part + j) != '\0'; j++) {
                    *(main_filename + i) = *(filename_part + j);
                    i++;
                }
                break;
            }
            i++;
        }

        dots_counter = 0, temp_dots_counter = 0;

        for(j = 0; *(ext_part + j) != '\0'; j++) {
            if(*(ext_part + j) == '.') dots_counter++;
        }

        for(j = 0; *(ext_part + j) != '\0'; j++) {
            if(*(ext_part + j) == '.') temp_dots_counter++;
            if(temp_dots_counter == dots_counter) {
                last_dot_index = j;
                break;
            }
        }

        for(j = last_dot_index; *(ext_part + j) != '\0'; j++) {
            *(main_filename + i) = *(ext_part + j);
            i++;
        }

        *(main_filename + i) = '\0';

        int res = save_image_t(main_filename, main_image);
        if(res != 0) {
            printf("Couldn't create file");
            for(i = 0; i < num_subimages; i++) {
                if(*(subimages + i) != NULL) destroy_image(&(*(subimages + i)));
            }
            free(subimages);
            destroy_image(&main_image);
            free(filename);
            free(main_filename);
            free(ext_part);
            return 5;
        }
        else {
            printf("File saved");
            for(i = 0; i < num_subimages; i++) {
                destroy_image(&(*(subimages + i)));
            }
            free(subimages);
            destroy_image(&main_image);
            free(filename);
            free(main_filename);
            free(ext_part);
            return 0;
        }
    }
}
