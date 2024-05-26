// 313 CAb Pungaru Stelian Andrei
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// free a matrix
void free_matrix(int ***a, int m)
{
	int i;
	for (i = 0; i < m; i++)
		free((*a)[i]);
	free((*a));
}
// allocation rgb matrix
void matrix_rgb(int ***a, int m, int n)
{
	int i;
	(*a) = (int **)malloc(m * sizeof(int *));
	for (i = 0; i < m; i++)
		(*a)[i] = (int *)malloc(3 * n * sizeof(int));
}
// allocation grayscale matrix
void matrix_gray(int ***a, int m, int n)
{
	int i;
	(*a) = (int **)malloc(m * sizeof(int *));
	for (i = 0; i < m; i++)
		(*a)[i] = (int *)malloc(n * sizeof(int));
}
// check the command
int verif_command(char command[20])
{
	if (!strcmp(command, "LOAD"))
		return 1;
	if (!strcmp(command, "SELECT"))
		return 1;
	if (!strcmp(command, "HISTOGRAM"))
		return 1;
	if (!strcmp(command, "EQUALIZE"))
		return 1;
	if (!strcmp(command, "ROTATE"))
		return 1;
	if (!strcmp(command, "CROP"))
		return 1;
	if (!strcmp(command, "APPLY"))
		return 1;
	if (!strcmp(command, "SAVE"))
		return 1;
	if (!strcmp(command, "EXIT"))
		return 0;
	return -1;
}
// check parameter for APPLY
int verif_parameter(char parameter[20])
{
	if (!strcmp(parameter, "EDGE"))
		return 1;
	if (!strcmp(parameter, "SHARPEN"))
		return 2;
	if (!strcmp(parameter, "BLUR"))
		return 3;
	if (!strcmp(parameter, "GAUSSIAN_BLUR"))
		return 4;
	return 0;
}
// convert a string of digits in decimal numbers
void convert_to_number(char x[20], int *a)
{
	int i;
	*a = 0;
	for (i = 0; i < (int)strlen(x); i++)
		*a = (*a) * 10 + (int)(x[i] - '0');
}

int min(int a, int b)
{
	if (a < b)
		return a;
	return b;
}
int max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

typedef struct
{
	char *type;
	int length, height, maxval;
	int **matrix;
} image;

void free_image(image **photo)
{
	free((*photo)->type);
	for (int i = 0; i < (*photo)->height; i++)
		free((*photo)->matrix[i]);
	free((*photo)->matrix);
	free((*photo));
}
// read from a text file with ascii pixels
void read_ascii(FILE *name, image **photo)
{
	int i, j;
	fgets((*photo)->type, 3, name);
	fscanf(name, "%d", &(*photo)->length);
	fscanf(name, "%d", &(*photo)->height);
	if ((*photo)->type[1] == '2' || (*photo)->type[1] == '3')
		fscanf(name, "%d", &(*photo)->maxval);
	// if the image is type p1 i don t read the maximum value of a pixel (=1)
	if ((*photo)->type[1] == '1')
		(*photo)->maxval = 1;
	if ((*photo)->type[1] == '1' || (*photo)->type[1] == '2')
		matrix_gray(&(*photo)->matrix, (*photo)->height, (*photo)->length);
	if ((*photo)->type[1] == '3')
		matrix_rgb(&(*photo)->matrix, (*photo)->height, (*photo)->length);
	if ((*photo)->type[1] == '1' || (*photo)->type[1] == '2')
	{
		for (i = 0; i < (*photo)->height; i++)
			for (j = 0; j < (*photo)->length; j++)
				fscanf(name, "%d", &(*photo)->matrix[i][j]);
	}
	if ((*photo)->type[1] == '3')
	{
		for (i = 0; i < (*photo)->height; i++)
			for (j = 0; j < 3 * (*photo)->length; j++)
			{
				fscanf(name, "%d", &(*photo)->matrix[i][j]);
			}
	}
}
// read pixels in binary format
void read_binary(FILE *name, image **photo)
{
	int i, j;
	unsigned char chr;
	fgets((*photo)->type, 3, name);
	fscanf(name, "%d", &(*photo)->length);
	fscanf(name, "%d", &(*photo)->height);
	if ((*photo)->type[1] == '5' || (*photo)->type[1] == '6')
		fscanf(name, "%d", &(*photo)->maxval);
	if ((*photo)->type[1] == '4')
		(*photo)->maxval = 1;
	if ((*photo)->type[1] == '4' || (*photo)->type[1] == '5')
		matrix_gray(&(*photo)->matrix, (*photo)->height, (*photo)->length);
	if ((*photo)->type[1] == '6')
		matrix_rgb(&(*photo)->matrix, (*photo)->height, (*photo)->length);
	fscanf(name, "\n");
	// read a byte = a channel
	if ((*photo)->type[1] == '4' || (*photo)->type[1] == '5')
	{
		for (i = 0; i < (*photo)->height; i++)
		{
			for (j = 0; j < (*photo)->length; j++)
			{
				fread(&chr, sizeof(char), 1, name);
				(*photo)->matrix[i][j] = (unsigned char)chr;
			}
		}
	}
	if ((*photo)->type[1] == '6')
	{
		for (i = 0; i < (*photo)->height; i++)
		{
			for (j = 0; j < 3 * (*photo)->length; j++)
			{
				fread(&chr, sizeof(char), 1, name);
				(*photo)->matrix[i][j] = (unsigned char)chr;
			}
		}
	}
}
// delete comments for ascii
void remove_comments_a(FILE *input_file, FILE *output_file)
{
	char line[1000];

	while (fgets(line, sizeof(line), input_file) != NULL)
	{
		if (line[0] != '#')
		{
			fputs(line, output_file);
		}
	}
}
// delete comments for binary
void remove_comments_b(FILE *input_file, FILE *output_file)
{
	int length, height;
	char buffer[1000], *w;
	unsigned char chr;
	// size_t bytesRead;
	fgets(buffer, sizeof(buffer), input_file);
	while (buffer[0] == '#')
		fgets(buffer, sizeof(buffer), input_file);
	fgets(buffer, sizeof(buffer), input_file);
	while (buffer[0] == '#')
		fgets(buffer, sizeof(buffer), input_file);
	fprintf(output_file, "%s", buffer);
	w = strtok(buffer, " ");
	convert_to_number(w, &length);
	w = strtok(NULL, " ");
	w[(int)strlen(w) - 1] = '\0';
	convert_to_number(w, &height);
	fgets(buffer, sizeof(buffer), input_file);
	while (buffer[0] == '#')
		fgets(buffer, sizeof(buffer), input_file);
	fprintf(output_file, "%s", buffer);
	while (fread(&chr, sizeof(char), 1, input_file))
		fwrite(&chr, sizeof(char), 1, output_file);
}
// pold is the main image, and photo the selected one
void select_image(image **photo, int x1, int x2, int y1, int y2, image *pold)
{
	int i, j;
	int **aux;
	// if image black and white or grayscale
	if ((*photo)->type[1] == '1' || (*photo)->type[1] == '2' || (*photo)->type[1] == '4' || (*photo)->type[1] == '5')
	{
		matrix_gray(&aux, y2 - y1, x2 - x1);
		for (i = 0; i < (y2 - y1); i++)
			for (j = 0; j < (x2 - x1); j++)
				aux[i][j] = pold->matrix[i + y1][j + x1];
		free_matrix(&(*photo)->matrix, (*photo)->height);
		matrix_gray(&(*photo)->matrix, y2 - y1, x2 - x1);
		for (i = 0; i < (y2 - y1); i++)
			for (j = 0; j < (x2 - x1); j++)
				(*photo)->matrix[i][j] = aux[i][j];
		(*photo)->height = y2 - y1;
		(*photo)->length = x2 - x1;
		free_matrix(&aux, y2 - y1);
	}
	if ((*photo)->type[1] == '3' || (*photo)->type[1] == '6')
	{
		matrix_rgb(&aux, y2 - y1, x2 - x1);
		for (i = 0; i < (y2 - y1); i++)
			for (j = 0; j < 3 * (x2 - x1); j++)
				aux[i][j] = pold->matrix[i + y1][j + 3 * x1];
		free_matrix(&(*photo)->matrix, (*photo)->height);
		matrix_rgb(&(*photo)->matrix, y2 - y1, x2 - x1);
		// triple number of columns for rgb
		for (i = 0; i < (y2 - y1); i++)
			for (j = 0; j < 3 * (x2 - x1); j++)
				(*photo)->matrix[i][j] = aux[i][j];
		(*photo)->height = y2 - y1;
		(*photo)->length = x2 - x1;
		free_matrix(&aux, y2 - y1);
	}
}

int check_if_power_2(int x)
{
	while (x > 1)
	{
		if (x % 2 == 1)
			return 0;
		x /= 2;
	}
	return 1;
}

void the_histogram(image *photo, int X, int Y)
{
	int i, j, nr_stars, nr_merged = (1 + photo->maxval) / Y, x[256] = {0}, y[256] = {0}, MAX = 0;
	// apply formula
	for (i = 0; i < photo->height; i++)
		for (j = 0; j < photo->length; j++)
			x[photo->matrix[i][j]]++;
	for (i = 0; i < Y; i++)
		for (j = 0; j < nr_merged; j++)
			y[i] += x[i * nr_merged + j];
	for (i = 0; i < Y; i++)
		if (y[i] > MAX)
			MAX = y[i];
	for (i = 0; i < Y; i++)
	{
		nr_stars = (int)y[i] * X / MAX;
		printf("%d\t|\t", nr_stars);
		for (j = 0; j < nr_stars; j++)
			printf("*");
		printf("\n");
	}
}
// check if a string contain only digits
int if_string_is_number(char x[100])
{
	int i = 0;
	while (i < (int)strlen(x) && x[i] >= '0' && x[i] <= '9')
		i++;
	if (i != (int)strlen(x))
		return 0;
	return 1;
}

int clamp(int x)
{
	if (x < 0)
		return 0;
	if (x > 255)
		return 255;
	return x;
}
// i update both images
void the_equalize(image **photo1, image **photo2)
{
	int i, j, k, h1[256] = {0}, h2[256] = {0};
	double s = 0;
	int **aux = NULL;
	matrix_gray(&aux, (*photo1)->height, (*photo1)->length);
	int area1 = (*photo1)->height * (*photo1)->length;
	int area2 = (*photo2)->height * (*photo2)->length;
	// calculate the frequency vector
	for (i = 0; i < (*photo1)->height; i++)
		for (j = 0; j < (*photo1)->length; j++)
			h1[(*photo1)->matrix[i][j]]++;
	for (i = 0; i < (*photo2)->height; i++)
		for (j = 0; j < (*photo2)->length; j++)
			h2[(*photo2)->matrix[i][j]]++;
	for (i = 0; i < (*photo1)->height; i++)
		for (j = 0; j < (*photo1)->length; j++)
		{
			s = 0;
			for (k = 0; k <= (*photo1)->matrix[i][j]; k++)
				s += h1[k];
			aux[i][j] = clamp((int)round(255 * s / area1));
		}
	for (i = 0; i < (*photo1)->height; i++)
		for (j = 0; j < (*photo1)->length; j++)
			(*photo1)->matrix[i][j] = aux[i][j];
	free_matrix(&aux, (*photo1)->height);
	matrix_gray(&aux, (*photo2)->height, (*photo2)->length);
	for (i = 0; i < (*photo2)->height; i++)
		for (j = 0; j < (*photo2)->length; j++)
		{
			s = 0;
			for (k = 0; k <= (*photo2)->matrix[i][j]; k++)
				s += h2[k];
			aux[i][j] = clamp((int)round(255 * s / area2));
		}
	for (i = 0; i < (*photo2)->height; i++)
		for (j = 0; j < (*photo2)->length; j++)
			(*photo2)->matrix[i][j] = aux[i][j];
	free_matrix(&aux, (*photo2)->height);
}

int **APPLY(image *init, image *photo, int parameter, int x1, int x2, int y1, int y2)
{
	int i, j, **aux;
	int s;
	matrix_rgb(&aux, photo->height, photo->length);
	for (i = y1; i < y2; i++)
	{
		for (j = 3 * x1; j < 3 * x2; j += 3)
		{
			// the edges of image1 remain the same
			if ((x1 == 0 && j == 0) || (y1 == 0 && i == 0) || (x2 == init->length && j == 3 * x2 - 3) || (y2 == init->height && i == y2 - 1))
			{
				aux[i - y1][j - 3 * x1] = init->matrix[i][j];
				aux[i - y1][j - 3 * x1 + 1] = init->matrix[i][j + 1];
				aux[i - y1][j - 3 * x1 + 2] = init->matrix[i][j + 2];
			}
			else
			{
				if (parameter == 1)
				{
					s = 0;
					s += -1 * init->matrix[i - 1][j - 3];
					s += -1 * init->matrix[i - 1][j];
					s += -1 * init->matrix[i - 1][j + 3];
					s += -1 * init->matrix[i][j - 3];
					s += 8 * init->matrix[i][j];
					s += -1 * init->matrix[i][j + 3];
					s += -1 * init->matrix[i + 1][j - 3];
					s += -1 * init->matrix[i + 1][j];
					s += -1 * init->matrix[i + 1][j + 3];
					aux[i - y1][j - 3 * x1] = clamp((int)round(s));
					s = 0;
					s += -1 * init->matrix[i - 1][j - 3 + 1];
					s += -1 * init->matrix[i - 1][j + 1];
					s += -1 * init->matrix[i - 1][j + 3 + 1];
					s += -1 * init->matrix[i][j - 3 + 1];
					s += 8 * init->matrix[i][j + 1];
					s += -1 * init->matrix[i][j + 3 + 1];
					s += -1 * init->matrix[i + 1][j - 3 + 1];
					s += -1 * init->matrix[i + 1][j + 1];
					s += -1 * init->matrix[i + 1][j + 3 + 1];
					aux[i - y1][j - 3 * x1 + 1] = clamp((int)round(s));
					s = 0;
					s += -1 * init->matrix[i - 1][j - 3 + 2];
					s += -1 * init->matrix[i - 1][j + 2];
					s += -1 * init->matrix[i - 1][j + 3 + 2];
					s += -1 * init->matrix[i][j - 3 + 2];
					s += 8 * init->matrix[i][j + 2];
					s += -1 * init->matrix[i][j + 3 + 2];
					s += -1 * init->matrix[i + 1][j - 3 + 2];
					s += -1 * init->matrix[i + 1][j + 2];
					s += -1 * init->matrix[i + 1][j + 3 + 2];
					aux[i - y1][j - 3 * x1 + 2] = clamp((int)round(s));
				}
				if (parameter == 2)
				{
					s = 0;
					s += 0 * init->matrix[i - 1][j - 3];
					s += -1 * init->matrix[i - 1][j];
					s += 0 * init->matrix[i - 1][j + 3];
					s += -1 * init->matrix[i][j - 3];
					s += 5 * init->matrix[i][j];
					s += -1 * init->matrix[i][j + 3];
					s += 0 * init->matrix[i + 1][j - 3];
					s += -1 * init->matrix[i + 1][j];
					s += 0 * init->matrix[i + 1][j + 3];
					aux[i - y1][j - 3 * x1] = clamp((int)round(s));
					s = 0;
					s += 0 * init->matrix[i - 1][j - 3 + 1];
					s += -1 * init->matrix[i - 1][j + 1];
					s += 0 * init->matrix[i - 1][j + 3 + 1];
					s += -1 * init->matrix[i][j - 3 + 1];
					s += 5 * init->matrix[i][j + 1];
					s += -1 * init->matrix[i][j + 3 + 1];
					s += 0 * init->matrix[i + 1][j - 3 + 1];
					s += -1 * init->matrix[i + 1][j + 1];
					s += 0 * init->matrix[i + 1][j + 3 + 1];
					aux[i - y1][j - 3 * x1 + 1] = clamp((int)round(s));
					s = 0;
					s += 0 * init->matrix[i - 1][j - 3 + 2];
					s += -1 * init->matrix[i - 1][j + 2];
					s += 0 * init->matrix[i - 1][j + 3 + 2];
					s += -1 * init->matrix[i][j - 3 + 2];
					s += 5 * init->matrix[i][j + 2];
					s += -1 * init->matrix[i][j + 3 + 2];
					s += 0 * init->matrix[i + 1][j - 3 + 2];
					s += -1 * init->matrix[i + 1][j + 2];
					s += 0 * init->matrix[i + 1][j + 3 + 2];
					aux[i - y1][j - 3 * x1 + 2] = clamp((int)round(s));
				}
				if (parameter == 3)
				{
					s = 0;
					s += init->matrix[i - 1][j - 3];
					s += init->matrix[i - 1][j];
					s += init->matrix[i - 1][j + 3];
					s += init->matrix[i][j - 3];
					s += init->matrix[i][j];
					s += init->matrix[i][j + 3];
					s += init->matrix[i + 1][j - 3];
					s += init->matrix[i + 1][j];
					s += init->matrix[i + 1][j + 3];
					s /= 9;
					aux[i - y1][j - 3 * x1] = clamp((int)round(s));
					s = 0;
					s += init->matrix[i - 1][j - 3 + 1];
					s += init->matrix[i - 1][j + 1];
					s += init->matrix[i - 1][j + 3 + 1];
					s += init->matrix[i][j - 3 + 1];
					s += init->matrix[i][j + 1];
					s += init->matrix[i][j + 3 + 1];
					s += init->matrix[i + 1][j - 3 + 1];
					s += init->matrix[i + 1][j + 1];
					s += init->matrix[i + 1][j + 3 + 1];
					s /= 9;
					aux[i - y1][j - 3 * x1 + 1] = clamp((int)round(s));
					s = 0;
					s += init->matrix[i - 1][j - 3 + 2];
					s += init->matrix[i - 1][j + 2];
					s += init->matrix[i - 1][j + 3 + 2];
					s += init->matrix[i][j - 3 + 2];
					s += init->matrix[i][j + 2];
					s += init->matrix[i][j + 3 + 2];
					s += init->matrix[i + 1][j - 3 + 2];
					s += init->matrix[i + 1][j + 2];
					s += init->matrix[i + 1][j + 3 + 2];
					s /= 9;
					aux[i - y1][j - 3 * x1 + 2] = clamp((int)round(s));
				}
				if (parameter == 4)
				{
					s = 0;
					s += init->matrix[i - 1][j - 3];
					s += 2 * init->matrix[i - 1][j];
					s += init->matrix[i - 1][j + 3];
					s += 2 * init->matrix[i][j - 3];
					s += 4 * init->matrix[i][j];
					s += 2 * init->matrix[i][j + 3];
					s += init->matrix[i + 1][j - 3];
					s += 2 * init->matrix[i + 1][j];
					s += init->matrix[i + 1][j + 3];
					s /= 16;
					aux[i - y1][j - 3 * x1] = clamp((int)round(s));
					s = 0;
					s += init->matrix[i - 1][j - 3 + 1];
					s += 2 * init->matrix[i - 1][j + 1];
					s += init->matrix[i - 1][j + 3 + 1];
					s += 2 * init->matrix[i][j - 3 + 1];
					s += 4 * init->matrix[i][j + 1];
					s += 2 * init->matrix[i][j + 3 + 1];
					s += init->matrix[i + 1][j - 3 + 1];
					s += 2 * init->matrix[i + 1][j + 1];
					s += init->matrix[i + 1][j + 3 + 1];
					s /= 16;
					aux[i - y1][j - 3 * x1 + 1] = clamp((int)round(s));
					s = 0;
					s += init->matrix[i - 1][j - 3 + 2];
					s += 2 * init->matrix[i - 1][j + 2];
					s += init->matrix[i - 1][j + 3 + 2];
					s += 2 * init->matrix[i][j - 3 + 2];
					s += 4 * init->matrix[i][j + 2];
					s += 2 * init->matrix[i][j + 3 + 2];
					s += init->matrix[i + 1][j - 3 + 2];
					s += 2 * init->matrix[i + 1][j + 2];
					s += init->matrix[i + 1][j + 3 + 2];
					s /= 16;
					aux[i - y1][j - 3 * x1 + 2] = clamp((int)round(s));
				}
			}
		}
	}
	for (i = 0; i < photo->height; i++)
		for (j = 0; j < 3 * photo->length; j++)
			photo->matrix[i][j] = aux[i][j];
	free_matrix(&aux, photo->height);
	return photo->matrix;
}

int main()
{
	// functiile de pointeri merg pe siruri de caractere dar nu pe numere
	image *photo1 = NULL, *photo2 = NULL;
	// photo1 is the cropped image and photo2 the selected image
	int a, x1, y1, x2, y2, x, y, loaded = 0, ok = 0, OK, i, j, X1, X2, Y2, Y1, direction, angle;
	char command[100], test[100], *word, w0rd[100];
	FILE *input, *output, *aux;
	fgets(command, sizeof(command), stdin);
	strcpy(test, command);
	word = strtok(test, " ");
	if (word[(int)strlen(word) - 1] == '\n')
		word[(int)strlen(word) - 1] = '\0';
	while (verif_command(word))
	{
		if (verif_command(word) == -1)
			printf("Invalid command\n");
		if (!strcmp(word, "LOAD"))
		{
			if (!strchr(command, ' '))
				printf("Invalid command\n");
			else
			{
				word = strtok(NULL, " ");
				if (word[(int)strlen(word) - 1] == '\n')
					word[(int)strlen(word) - 1] = '\0';
				if (strcmp(word, "\0") == 0)
					printf("Invalid command\n");
				// if there is no space or enter after it => invalid
				else
				{
					strcpy(test, command);
					if (test[(int)strlen(test) - 1] == '\n')
						test[(int)strlen(test) - 1] = '\0';
					input = fopen(test + 5, "r");
					if (input)
					{
						if (loaded)
						{
							if (photo1)
								free_image(&photo1);
							if (photo2)
								free_image(&photo2);
						}
						loaded = 1;
						// read a character and if it is the end of file, the file is empty
						int g = fgetc(input);
						if (g == EOF)
							loaded = 0;
						if (loaded)
						{
							photo2 = (image *)malloc(sizeof(image));
							photo2->type = (char *)malloc(3 * sizeof(char));
							// recover first character
							fseek(input, -1, SEEK_CUR);
							fscanf(input, "%s", photo2->type);
							while (photo2->type[0] == '#')
								fscanf(input, "%s", photo2->type);
							// mida and midb are first images without comments
							if (photo2->type[1] == '1' || photo2->type[1] == '2' || photo2->type[1] == '3')
							{
								aux = fopen("mida", "w");
								fprintf(aux, "%s", photo2->type);
								remove_comments_a(input, aux);
								fclose(input);
								fclose(aux);
								aux = fopen("mida", "r");
								read_ascii(aux, &photo2);
							}
							else
							{
								aux = fopen("midb", "w");
								fprintf(aux, "%s\n", photo2->type);
								remove_comments_b(input, aux);
								fclose(input);
								fclose(aux);
								aux = fopen("midb", "r");
								read_binary(aux, &photo2);
							}
							photo1 = (image *)malloc(sizeof(image));
							photo1->type = (char *)malloc(3 * sizeof(char));
							strcpy(photo1->type, photo2->type);
							photo1->length = photo2->length;
							photo1->height = photo2->height;
							// read the images type ahead separately to select which array type should be dynamically allocated
							if (photo1->type[1] == '1' || photo1->type[1] == '2' || photo1->type[1] == '4' || photo1->type[1] == '5')
							{
								matrix_gray(&photo1->matrix, photo1->height, photo1->length);
								for (i = 0; i < photo1->height; i++)
									for (j = 0; j < photo1->length; j++)
										photo1->matrix[i][j] = photo2->matrix[i][j];
							}
							if (photo1->type[1] == '3' || photo1->type[1] == '6')
							{
								matrix_rgb(&photo1->matrix, photo1->height, photo1->length);
								for (i = 0; i < photo1->height; i++)
									for (j = 0; j < 3 * photo1->length; j++)
										photo1->matrix[i][j] = photo2->matrix[i][j];
							}
							photo1->maxval = photo2->maxval;
							printf("Loaded ");
							puts(test + 5);
							fclose(aux);
						}
						else
						{
							printf("Failed to load ");
							puts(test + 5);
						}
					}
					else
					{
						loaded = 0;
						printf("Failed to load ");
						puts(test + 5);
					}
				}
			}
		}
		if (!strcmp(word, "SELECT"))
		{
			ok = 0;
			if (!strchr(command, ' '))
				printf("Invalid command\n");
			else
			{
				if (loaded == 0)
					printf("No image loaded\n");
				else
				{
					word = strtok(NULL, " ");
					if (word[(int)strlen(word) - 1] == '\n')
						word[(int)strlen(word) - 1] = '\0';
					if (!strcmp(word, "ALL"))
					{
						printf("Selected ALL\n");
						strcpy(photo2->type, photo1->type);
						free_matrix(&photo2->matrix, photo2->height);
						photo2->length = photo1->length;
						photo2->height = photo1->height;
						// x1,x2,y1,y2  are the coordinates to the current selection
						// X1,X2,Y1,Y2 are the coordinates to the last selection made successfully
						x1 = 0;
						y1 = 0;
						x2 = photo1->length;
						y2 = photo1->height;
						X1 = x1;
						X2 = x2;
						Y1 = y1;
						Y2 = y2;
						if (photo2->type[1] == '3' || photo2->type[1] == '6')
						{
							matrix_rgb(&photo2->matrix, photo2->height, photo2->length);
							for (i = 0; i < photo2->height; i++)
								for (j = 0; j < 3 * photo2->length; j++)
									photo2->matrix[i][j] = photo1->matrix[i][j];
						}
						else
						{
							matrix_gray(&photo2->matrix, photo2->height, photo2->length);
							for (i = 0; i < photo2->height; i++)
								for (j = 0; j < photo2->length; j++)
									photo2->matrix[i][j] = photo1->matrix[i][j];
						}
						if (photo2->type[1] == '1' || photo2->type[1] == '2' || photo2->type[1] == '4' || photo2->type[1] == '5')
						{
							for (i = 0; i < photo2->height; i++)
								for (j = 0; j < photo2->length; j++)
									photo2->matrix[i][j] = photo1->matrix[i][j];
						}
						if (photo2->type[1] == '3' || photo2->type[1] == '6')
						{
							for (i = 0; i < photo2->height; i++)
								for (j = 0; j < 3 * photo2->length; j++)
									photo2->matrix[i][j] = photo1->matrix[i][j];
						}
						photo2->maxval = photo1->maxval;
						// for select all both image are equal
					}
					else
					{
						if (strcmp(word, "\0") == 0)
							printf("Invalid command\n");
						else
						{
							OK = 0;
							if (if_string_is_number(word))
							{
								convert_to_number(word, &x1);
								word = strtok(NULL, " ");
								if (!word || strchr(word, '\n'))
								{
									printf("Invalid command\n");
									OK = 1;
								}
								else if (if_string_is_number(word))
								{
									convert_to_number(word, &y1);
									word = strtok(NULL, " ");
									if (!word || strchr(word, '\n'))
									{
										printf("Invalid command\n");
										OK = 1;
									}
									else if (if_string_is_number(word))
									{
										convert_to_number(word, &x2);
										word = strtok(NULL, " ");
										if (word[(int)strlen(word) - 1] == '\n')
											word[(int)strlen(word) - 1] = '\0';
										if (!word || !if_string_is_number(word))
										{
											printf("Invalid command\n");
											OK = 1;
										}
										else
										{
											convert_to_number(word, &y2);
											a = x1;
											x1 = min(x1, x2);
											x2 = max(a, x2);
											a = y1;
											y1 = min(y1, y2);
											y2 = max(a, y2);
											word = strtok(NULL, " ");
											if (!word && x1 != x2 && y1 != y2 && x1 >= 0 && y1 >= 0 && x1 < photo1->length && y1 < photo1->height && x2 > 0 && y2 > 0 && x2 <= photo1->length && y2 <= photo1->height)
											{
												ok = 1;
												X1 = x1;
												X2 = x2;
												Y1 = y1;
												Y2 = y2;
											}
										}
									}
								}
							}
							// OK =1 if all the coordonates are numbers and ok=1 if they respects the conditions
							if (OK == 0)
							{
								if (!loaded)
									printf("No image loaded\n");
								else
								{
									if (!ok)
										printf("Invalid set of coordinates\n");
									else
									{
										free_matrix(&photo2->matrix, photo2->height);
										photo2->length = photo1->length;
										photo2->height = photo1->height;
										if (photo2->type[1] == '3' || photo2->type[1] == '6')
											matrix_rgb(&photo2->matrix, photo2->height, photo2->length);
										else
											matrix_gray(&photo2->matrix, photo2->height, photo2->length);
										select_image(&photo2, x1, x2, y1, y2, photo1);
										printf("Selected ");
										printf("%d %d %d %d\n", x1, y1, x2, y2);
									}
								}
							}
							strcpy(test, command);
							word = strtok(test, " ");
							if (word[(int)strlen(word) - 1] == '\n')
								word[(int)strlen(word) - 1] = '\0';
						}
					}
				}
			}
		}
		if (!strcmp(word, "HISTOGRAM"))
		{
			if (loaded == 0)
				printf("No image loaded\n");
			else
			{
				if (!strchr(command, ' '))
					printf("Invalid command\n");
				else
				{
					if (photo2->type[1] == '3' || photo2->type[1] == '6')
						printf("Black and white image needed\n");
					else
					{
						word = strtok(NULL, " ");
						if (!strcmp(word, "\0") || if_string_is_number(word) == 0)
							printf("Invalid command\n");
						else
						{
							if (word[(int)strlen(word) - 1] == '\n')
								printf("Invalid command\n");
							else
							{
								convert_to_number(word, &x);
								word = strtok(NULL, " ");
								if (word[(int)strlen(word) - 1] == '\n')
									word[(int)strlen(word) - 1] = '\0';
								if (!strcmp(word, "\0") || if_string_is_number(word) == 0)
									printf("Invalid command\n");
								else
								{
									convert_to_number(word, &y);
									if (y < 2 || y > 256 || !check_if_power_2(y))
										printf("Invalid command\n");
									else
									{
										if (strtok(NULL, " "))
											printf("Invalid command\n");
										else
										{
											the_histogram(photo2, x, y);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if (!strcmp(word, "EQUALIZE"))
		{
			if (!loaded)
				printf("No image loaded\n");
			else
			{
				if (strchr(command, ' '))
					printf("Invalid command\n");
				else
				{
					if (photo1->type[1] == '3' || photo1->type[1] == '6')
						printf("Black and white image needed\n");
					else
					{
						the_equalize(&photo1, &photo2);
						printf("Equalize done\n");
					}
				}
			}
			strcpy(test, command);
			word = strtok(test, " ");
		}
		if (!strcmp(word, "ROTATE"))
		{
			if (!strchr(command, ' '))
				printf("Invalid command\n");
			else
			{
				if (!loaded)
					printf("No image loaded\n");
				else
				{
					word = strtok(NULL, " ");
					direction = 0;
					if (word[(int)strlen(word) - 1] == '\n')
						word[(int)strlen(word) - 1] = '\0';
					else
						printf("Invalid command\n");
					if (word[0] == '-')
					{
						word[0] = '0';
						direction = -1;
					}
					if (!direction)
						direction = 1;
					if (if_string_is_number(word))
						convert_to_number(word, &angle);
					else
						printf("Unsupported rotation angle\n");
					if (strtok(NULL, " ") || !(angle >= -360 && angle <= 360 && abs(angle) % 90 == 0))
						printf("Unsupported rotation angle\n");

					else
					{
						// if the selected all rotate must happen
						if (photo2->height != photo2->length && photo2->height != photo1->height && photo2->height != photo1->height)
							printf("The selection must be square\n");
						else
						{
							if (direction == 1)
								printf("Rotated %d\n", angle);
							if (direction == -1)
								printf("Rotated -%d\n", angle);
						}
					}
				}
			}
		}
		if (!strcmp(word, "CROP"))
		{
			if (strchr(command, ' '))
				printf("Invalid command\n");
			else
			{
				if (!loaded)
					printf("No image loaded\n");
				else
				{
					// copy photo2 in photo1
					strcpy(photo1->type, photo2->type);
					free_matrix(&photo1->matrix, photo1->height);
					photo1->length = photo2->length;
					photo1->height = photo2->height;
					if (photo1->type[1] == '1' || photo1->type[1] == '2' || photo1->type[1] == '4' || photo1->type[1] == '5')
					{
						matrix_gray(&photo1->matrix, photo1->height, photo1->length);
						for (i = 0; i < photo1->height; i++)
							for (j = 0; j < photo1->length; j++)
								photo1->matrix[i][j] = photo2->matrix[i][j];
					}
					if (photo1->type[1] == '3' || photo1->type[1] == '6')
					{
						matrix_rgb(&photo1->matrix, photo1->height, photo1->length);
						for (i = 0; i < photo1->height; i++)
							for (j = 0; j < 3 * photo1->length; j++)
								photo1->matrix[i][j] = photo2->matrix[i][j];
					}
					photo1->maxval = photo2->maxval;
					printf("Image cropped\n");
				}
			}
			strcpy(test, command);
			word = strtok(test, " ");
		}
		if (!strcmp(word, "APPLY"))
		{
			if (!loaded)
				printf("No image loaded\n");
			else
			{
				if (!strchr(command, ' '))
					printf("Invalid command\n");
				else
				{
					word = strtok(NULL, " ");
					if (word[strlen(word) - 1] != '\n')
						printf("APPLY parameter invalid\n");
					else
					{
						if (photo2->type[1] == '1' || photo2->type[1] == '2' || photo2->type[1] == '4' || photo2->type[1] == '5')
							printf("Easy, Charlie Chaplin\n");
						else
						{
							word[strlen(word) - 1] = '\0';
							if (!verif_parameter(word))
								printf("APPLY parameter invalid\n");
							if (verif_parameter(word))
							{
								photo2->matrix = APPLY(photo1, photo2, verif_parameter(word), X1, X2, Y1, Y2);
								printf("APPLY %s done\n", word);
								for (i = Y1; i < Y2; i++)
									for (j = 3 * X1; j < 3 * X2; j++)
										photo1->matrix[i][j] = photo2->matrix[i - Y1][j - 3 * X1];
							}
						}
					}
				}
			}
		}
		if (!strcmp(word, "SAVE"))
		{
			if (!strchr(command, ' '))
				printf("Invalid command\n");
			else
			{
				word = strtok(NULL, " ");
				if (word[(int)strlen(word) - 1] == '\n')
					word[(int)strlen(word) - 1] = '\0';
				if (strcmp(word, "\0") == 0)
					printf("Invalid command\n");
				else
				{
					strcpy(test, command);
					if (test[(int)strlen(test) - 1] == '\n')
						test[(int)strlen(test) - 1] = '\0';
					word = strtok(test, " ");
					word = strtok(NULL, " ");
					output = fopen(word, "w");
					strcpy(w0rd, word);
					if (output)
					{
						if (loaded)
						{
							int O_K = 0;
							if (!strchr(command + 5, ' ') || !strtok(NULL, " "))
							{
								// create txt file with binary elements
								if (photo1->type[1] == '1' || photo1->type[1] == '4')
									fprintf(output, "P4\n");
								if (photo1->type[1] == '2' || photo1->type[1] == '5')
									fprintf(output, "P5\n");
								if (photo1->type[1] == '3' || photo1->type[1] == '6')
									fprintf(output, "P6\n");
								fprintf(output, "%d %d\n", photo1->length, photo1->height);
								if (photo1->type[1] == '2' || photo1->type[1] == '3' || photo1->type[1] == '5' || photo1->type[1] == '6')
									fprintf(output, "%d\n", photo1->maxval);
								if (photo1->type[1] == '1' || photo1->type[1] == '2' || photo1->type[1] == '4' || photo1->type[1] == '5')
								{
									for (i = 0; i < photo1->height; i++)
									{
										for (j = 0; j < photo1->length; j++)
											fwrite(&photo1->matrix[i][j], sizeof(char), 1, output);
									}
								}
								if (photo1->type[1] == '3' || photo1->type[1] == '6')
								{
									for (i = 0; i < photo1->height; i++)
									{
										for (j = 0; j < 3 * photo1->length; j++)
											fwrite(&photo1->matrix[i][j], sizeof(char), 1, output);
									}
								}
								printf("Saved ");
								puts(w0rd);
								fclose(output);
								O_K = 1;
							}
							if (strchr(command + 5, ' ') && O_K == 0)
							{
								strcpy(test, command);
								if (test[(int)strlen(test) - 1] == '\n')
									test[(int)strlen(test) - 1] = '\0';
								word = strtok(test, " ");
								word = strtok(NULL, " ");
								word = strtok(NULL, " ");
								// printf("haicu%stata",word);
								if (word[(int)strlen(word) - 1] == '\n')
									word[(int)strlen(word) - 1] = '\0';
								if (!strcmp(word, "ascii") && !strtok(NULL, " "))
								{
									// save in format ascii
									if (photo1->type[1] == '1' || photo1->type[1] == '4')
										fprintf(output, "P1\n");
									if (photo1->type[1] == '2' || photo1->type[1] == '5')
										fprintf(output, "P2\n");
									if (photo1->type[1] == '3' || photo1->type[1] == '6')
										fprintf(output, "P3\n");
									fprintf(output, "%d %d\n", photo1->length, photo1->height);
									if (photo1->type[1] == '2' || photo1->type[1] == '3' || photo1->type[1] == '5' || photo1->type[1] == '6')
										fprintf(output, "%d\n", photo1->maxval);
									if (photo1->type[1] == '1' || photo1->type[1] == '2' || photo1->type[1] == '4' || photo1->type[1] == '5')
									{
										for (i = 0; i < photo1->height; i++)
										{
											for (j = 0; j < photo1->length; j++)
												fprintf(output, "%d ", photo1->matrix[i][j]);
											fprintf(output, "\n");
										}
									}
									if (photo1->type[1] == '3' || photo1->type[1] == '6')
									{
										for (i = 0; i < photo1->height; i++)
										{
											for (j = 0; j < 3 * photo1->length; j++)
												fprintf(output, "%d ", photo1->matrix[i][j]);
											fprintf(output, "\n");
										}
									}
									printf("Saved ");
									puts(w0rd);
									fclose(output);
								}
								if (strtok(NULL, " "))
									printf("Invalid command\n");
							}
						}
						else
							printf("No image loaded\n");
					}
					else
					{
						printf("Invalid command\n");
					}
				}
			}
			strcpy(test, command);
			word = strtok(test, " ");
		}
		fgets(command, sizeof(command), stdin);
		strcpy(test, command);
		word = strtok(test, " ");
		if (word[(int)strlen(word) - 1] == '\n')
			word[(int)strlen(word) - 1] = '\0';
	}
	if (loaded == 0)
		printf("No image loaded\n");
	else
	{
		if (photo1)
			free_image(&photo1);
		if (photo2)
			free_image(&photo2);
	}
	return 0;
}
