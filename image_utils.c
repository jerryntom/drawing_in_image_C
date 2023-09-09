#include "image_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct image_t* load_image_t(const char *filename, int *err_code) {
    if(filename == NULL) {
        if(err_code != NULL) *err_code = 1;
        return NULL;
    }

    char *type = malloc(sizeof(char) * 3);
    if(type == NULL) {
        free(type);
        if(err_code != NULL) *err_code = 4;
        return NULL;
    }

    int width, height, i, j, k, value;

    FILE *fin = fopen(filename, "rt");
    if(fin == NULL) {
        if(err_code != NULL) *err_code = 2;
        free(type);
        return NULL;
    }

    int chars_amount = 0, ch;

    while(1) {
        ch = fgetc(fin);
        if(ch == '\n') break;
        chars_amount++;
    }

    if(chars_amount > 2) {
        free(type);
        fclose(fin);
        if(err_code != NULL) *err_code = 3;
        return NULL;
    }

    fseek(fin, 0, SEEK_SET);

    struct image_t *image = malloc(sizeof(struct image_t));
    if(image == NULL) {
        free(type);
        fclose(fin);
        if(err_code != NULL) *err_code = 4;
        return NULL;
    }

    fscanf(fin, "%s", type);
    if(strcmp(type, "P2") == 0) {
        strcpy(image->type, type);
        free(type);
    }
    else {
        free(image);
        free(type);
        fclose(fin);
        if(err_code != NULL) *err_code = 3;
        return NULL;
    }

    if(fscanf(fin, "%d %d", &width, &height) != 2) {
        free(image);
        fclose(fin);
        if(err_code != NULL) *err_code = 3;
        return NULL;
    }

    if(width <= 0 || height <= 0) {
        free(image);
        fclose(fin);
        if(err_code != NULL) *err_code = 3;
        return NULL;
    }
    else {
        image->width = width;
        image->height = height;
    }

    int max_file_value;
    fscanf(fin, "%d", &max_file_value);

    if(max_file_value <= 0) {
        free(image);
        fclose(fin);
        if(err_code != NULL) *err_code = 3;
        return NULL;
    }

    image->ptr = malloc(height * sizeof(int *));
    if(image->ptr == NULL) {
        free(image);
        fclose(fin);
        if(err_code != NULL) *err_code = 4;
        return NULL;
    }

    for(i = 0; i < height; i++) {
        *(image->ptr + i) = malloc(width * sizeof(int));
        if(*(image->ptr + i) == NULL) {
            for(j = 0; j < i; j++) {
                free(*(image->ptr + j));
            }
            free(image->ptr);
            free(image);
            fclose(fin);
            if(err_code != NULL) *err_code = 4;
            return NULL;
        }
    }

    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++) {
            if(fscanf(fin, "%d", &value) != 1 || value > max_file_value || value < 0) {
                for(k = 0; k < height; k++) {
                    free(*(image->ptr + k));
                }
                free(image->ptr);
                free(image);
                fclose(fin);
                if(err_code != NULL) *err_code = 3;
                return NULL;
            }
            else *(*(image->ptr + i) + j) = value;
        }
    }

    if(err_code != NULL) *err_code = 0;
    fclose(fin);
    return image;
}

int save_image_t(const char * filename, const struct image_t *m1) {
    if(filename == NULL || m1 == NULL || m1->ptr == NULL
       || m1->height <= 0 || m1->width <= 0) return 1;

    int value, i, j;

    FILE *fout = fopen(filename, "wt");
    if(fout == NULL) return 2;

    if(fprintf(fout, "%s\n", m1->type) < 0) {
        fclose(fout);
        return 3;
    }

    if(fprintf(fout, "%d %d\n", m1->width, m1->height) < 0) {
        fclose(fout);
        return 3;
    }

    if(fprintf(fout, "255\n") < 0) {
        fclose(fout);
        return 3;
    }

    for(i = 0; i < m1->height; i++) {
        for(j = 0; j < m1->width; j++) {
            value = *(*(m1->ptr + i) + j);
            if(j != m1->width - 1) {
                if(fprintf(fout, "%d ", value) < 0) {
                    fclose(fout);
                    return 3;
                }
            }
            else {
                if(j == m1->width - 1 && i != m1->height - 1) {
                    if(fprintf(fout, "%d\n", value) < 0) {
                        fclose(fout);
                        return 3;
                    }
                }
                else if(j == m1->width - 1 && i == m1->height - 1) {
                    if(fprintf(fout, "%d", value) < 0) {
                        fclose(fout);
                        return 3;
                    }
                }
            }
        }
    }

    fclose(fout);
    return 0;
}

void destroy_image(struct image_t **m) {
    if(m == NULL || *m == NULL) return;
    if((*m)->height <= 0 || (*m)->width <= 0 || (*m)->ptr == NULL) return;

    int i;

    for(i = 0; i < (*m)->height; i++) {
        free(*(((*m)->ptr) + i));
        *(((*m)->ptr) + i) = NULL;
    }

    free((*m)->ptr);
    free(*m);
    *m = NULL;
}

const int* image_get_pixel(const struct image_t *img, int x, int y) {
    if(img == NULL || img->ptr == NULL || x < 0 || y < 0) return NULL;
    if(x >= img->width || y >= img->height) return NULL;

    return (*(img->ptr + y) + x);
}

int* image_set_pixel(struct image_t *img, int x, int y) {
    if(img == NULL || img->ptr == NULL || x < 0 || y < 0) return NULL;
    if(x >= img->width || y >= img->height) return NULL;

    return (*(img->ptr + y) + x);
}

int draw_image(struct image_t *img, const struct image_t *src, int destx, int desty) {
    if(img == NULL || src == NULL || destx < 0 || desty < 0) return 1;
    if(destx >= img->width || desty >= img->height) return 1;
    if(img->height <= 0 || img->width <= 0 || src->height <= 0 || src->width <= 0) return 1;
    if(src->width > img->width || src->height > img->height) return 1;
    if((img->width - destx) < src->width || (img->height - desty) < src->height) return 1;

    int dst_i = 0, dst_j = 0, src_i = 0, src_j = 0, value;

    while(1) {
        if(dst_i == 0 && dst_j == 0) {
            dst_i = desty;
            dst_j = destx;
        }
        if(dst_i >= img->height) return 1;

        value = *(*(src->ptr + src_i) + src_j);
        *(*(img->ptr + dst_i) + dst_j) = value;

        src_j++;
        dst_j++;

        if(src_j == src->width) {
            src_i++;
            src_j = 0;
            dst_i++;
            dst_j = destx;
            if(src_i == src->height) return 0;
        }
    }
}
