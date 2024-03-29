#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Para usar strings

#include <math.h> // Funções matemáticas

#ifdef WIN32
#include <windows.h> // Apenas para Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>   // Funções da OpenGL
#include <GL/glu.h>  // Funções da GLU
#include <GL/glut.h> // Funções da FreeGLUT
#endif

// SOIL é a biblioteca para leitura das imagens
#include <SOIL.h>

// Um pixel RGB (24 bits)
typedef struct
{
    unsigned char r, g, b;
} RGB8;

// Uma imagem RGB
typedef struct
{
    int width, height;
    RGB8 *img;
} Img;

// Protótipos
void load(char *name, Img *pic);
void uploadTexture();
void seamcarve(int targetWidth); // executa o algoritmo
void freemem();                  // limpa memória (caso tenha alocado dinamicamente)

// Funções da interface gráfica e OpenGL
void init();
void draw();
void keyboard(unsigned char key, int x, int y);
void arrow_keys(int a_keys, int x, int y);

// Largura e altura da janela
int width, height;

// Largura desejada (selecionável)
int targetW;

// Identificadores de textura
GLuint tex[3];

// As 3 imagens
Img pic[3];
Img *source;
Img *mask;
Img *target;

// Imagem selecionada (0,1,2)
int sel;

// Carrega uma imagem para a struct Img
void load(char *name, Img *pic)
{
    int chan;
    pic->img = (RGB8 *)SOIL_load_image(name, &pic->width, &pic->height, &chan, SOIL_LOAD_RGB);
    if (!pic->img)
    {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
        exit(1);
    }
    printf("Load: %d x %d x %d\n", pic->width, pic->height, chan);
}

//
// Implemente AQUI o seu algoritmo
void seamcarve(int targetWidth)
{
    // Aplica o algoritmo e gera a saida em target->img...

    // Ponteiro para a imagem de entrada.
    RGB8(*ptrSource)[source->width] = (RGB8(*)[source->width])source->img;

    // Ponteiro para a imagem mask.
    RGB8(*ptrMask)[mask->width] = (RGB8(*)[mask->width])mask->img;

    // Ponteiro para a imagem de saída.
    RGB8(*ptrTarget)[target->width] = (RGB8(*)[target->width])target->img;

    // Matriz de energias dos pixels.
    int energies[target->height][target->width];

    // Matriz de energias acumuladas.
    int acumulatedEnergies[target->height][target->width];

    // Preenchendo a matriz de energias acumuladas com valores grandes. 
    for(int y = 0; y < target->height; y++){
        for(int x = 0; x < target->width; x++){
            acumulatedEnergies[y][x] = 999999999;
        }
    }

    // Preenchendo o vetor target com os pixels de source e calculando as energias dos pixels.
    for (int y = 1; y < target->height-1; y++){
        for (int x = 1; x < target->width-1; x++){

            ptrTarget[y][x].r = ptrSource[y][x].r;
            ptrTarget[y][x].g = ptrSource[y][x].g;
            ptrTarget[y][x].b = ptrSource[y][x].b;

            int pixelEnergy;

            // Se na mask o pixel correspondente for verde ou vermelho,
            // então atribuimos um valor de energia manualmente, caso o
            // contrário, o cálculo de energia é feito.
            if(!(ptrMask[y][x].r == 255 && ptrMask[y][x].g == 255 && ptrMask[y][x].b == 255)){
                // Verde
                if(ptrMask[y][x].g >= 200){
                    pixelEnergy = 50000;
                }
                // Vermelho
                else{
                    pixelEnergy = -50000;
                }
            }
            else{
                int deltaRX = ptrSource[y][x-1].r - ptrSource[y][x+1].r; 
                int deltaGX = ptrSource[y][x-1].g - ptrSource[y][x+1].g;
                int deltaBX = ptrSource[y][x-1].b - ptrSource[y][x+1].b;
                int deltaX = pow(deltaRX, 2) + pow(deltaGX, 2) + pow(deltaBX, 2);

                int deltaRY = ptrSource[y-1][x].r - ptrSource[y+1][x].r;
                int deltaGY = ptrSource[y-1][x].g - ptrSource[y+1][x].g;
                int deltaBY = ptrSource[y-1][x].b - ptrSource[y+1][x].b;
                int deltaY = pow(deltaRY, 2) + pow(deltaGY, 2) + pow(deltaBY, 2);

                pixelEnergy = deltaX + deltaY;
            }
  
            energies[y][x] = pixelEnergy;

            if(y == 1){
                acumulatedEnergies[y][x] = pixelEnergy;
            }

        }     
    }

    // Preenchendo o vetor de energias acumuladas.
    for (int y = 1; y < target->height-1; y++){
        for (int x = 1; x < target->width-1; x++){
            int currentEnergy = acumulatedEnergies[y][x];

            int energyLeft = energies[y+1][x-1];
            int energyMiddle = energies[y+1][x];
            int energyRight = energies[y+1][x+1];

            int acumulatedEnergyLeft = acumulatedEnergies[y+1][x-1];
            int acumulatedEnergyMiddle = acumulatedEnergies[y+1][x];
            int acumulatedEnergyRight = acumulatedEnergies[y+1][x+1];
            
            if(currentEnergy + energyLeft < acumulatedEnergyLeft){
                acumulatedEnergies[y+1][x-1] = currentEnergy + energyLeft;
            }
            if(currentEnergy + energyMiddle < acumulatedEnergyMiddle){
                acumulatedEnergies[y+1][x] = currentEnergy + energyMiddle;
            }
            if(currentEnergy + energyRight < acumulatedEnergyRight){
                acumulatedEnergies[y+1][x+1] = currentEnergy + energyRight;
            }
        }
    }

    int width = target->width;

    // Cortando linhas de pixels até chegarmos ao valor de largura da imagem desejado.
    while(width > targetWidth){

        // Achando o menor valor da última linha da matriz de energias acumuladas.
        int lowest = 999999999;
        int columnOfLowest = -1;

        for(int x = 1; x < width-1; x++){
            int currentValue = acumulatedEnergies[target->height-2][x];
            if(currentValue < lowest){
                lowest = currentValue;
                columnOfLowest = x;
            }
        }

        // Começar o percorrimento de baixo para cima para formar a linha de corte.
        int i = target->height-2;
        int j = columnOfLowest;

        while(i > 0){

            int k = i;
            int l = j;

            // Pintando os pixels atuais de preto.
            ptrTarget[i][j].r = 0;
            ptrTarget[i][j].g = 0;
            ptrTarget[i][j].b = 0;

            // Definindo valores grandes para a energia e energia acumulada daquele pixel.
            energies[i][j] = 999999999;
            acumulatedEnergies[i][j] = 999999999;

            int energyLeft = acumulatedEnergies[i-1][j-1];
            int energyMiddle = acumulatedEnergies[i-1][j];
            int energyRight = acumulatedEnergies[i-1][j+1];

            if(energyLeft <= energyMiddle && energyLeft <= energyRight){
                j--;
            }
            else if(energyRight <= energyMiddle && energyRight <= energyLeft){
                j++;
            }

            while(l < width-1){

                // Deslocando os pixels de target.
                RGB8 aux = ptrTarget[k][l];
                ptrTarget[k][l] = ptrTarget[k][l+1];
                ptrTarget[k][l+1] = aux;

                // Deslocando as energias
                int aux2 = energies[k][l];
                energies[k][l] = energies[k][l+1];
                energies[k][l+1] = aux2;

                // Deslocando as energias acumuladas
                int aux3 = acumulatedEnergies[k][l];
                acumulatedEnergies[k][l] = acumulatedEnergies[k][l+1];
                acumulatedEnergies[k][l+1] = aux3;

                l++;
            }
            
            i--;

        }
        width--;
    }

    // Chame uploadTexture a cada vez que mudar
    // a imagem (pic[2])
    uploadTexture();
    glutPostRedisplay();
}

void freemem()
{
    // Libera a memória ocupada pelas 3 imagens
    free(pic[0].img);
    free(pic[1].img);
    free(pic[2].img);
}

/********************************************************************
 * 
 *  VOCÊ NÃO DEVE ALTERAR NADA NO PROGRAMA A PARTIR DESTE PONTO!
 *
 ********************************************************************/
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("seamcarving [origem] [mascara]\n");
        printf("Origem é a imagem original, mascara é a máscara desejada\n");
        exit(1);
    }
    glutInit(&argc, argv);

    // Define do modo de operacao da GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // pic[0] -> imagem original
    // pic[1] -> máscara desejada
    // pic[2] -> resultado do algoritmo

    // Carrega as duas imagens
    load(argv[1], &pic[0]);
    load(argv[2], &pic[1]);

    if (pic[0].width != pic[1].width || pic[0].height != pic[1].height)
    {
        printf("Imagem e máscara com dimensões diferentes!\n");
        exit(1);
    }

    // A largura e altura da janela são calculadas de acordo com a maior
    // dimensão de cada imagem
    width = pic[0].width;
    height = pic[0].height;

    // A largura e altura da imagem de saída são iguais às da imagem original (1)
    pic[2].width = pic[1].width;
    pic[2].height = pic[1].height;

    // Ponteiros para as structs das imagens, para facilitar
    source = &pic[0];
    mask = &pic[1];
    target = &pic[2];

    // Largura desejada inicialmente é a largura da janela
    targetW = target->width;

    // Especifica o tamanho inicial em pixels da janela GLUT
    glutInitWindowSize(width, height);

    // Cria a janela passando como argumento o titulo da mesma
    glutCreateWindow("Seam Carving");

    // Registra a funcao callback de redesenho da janela de visualizacao
    glutDisplayFunc(draw);

    // Registra a funcao callback para tratamento das teclas ASCII
    glutKeyboardFunc(keyboard);

    // Registra a funcao callback para tratamento das setas
    glutSpecialFunc(arrow_keys);

    // Cria texturas em memória a partir dos pixels das imagens
    tex[0] = SOIL_create_OGL_texture((unsigned char *)pic[0].img, pic[0].width, pic[0].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
    tex[1] = SOIL_create_OGL_texture((unsigned char *)pic[1].img, pic[1].width, pic[1].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Exibe as dimensões na tela, para conferência
    printf("Origem  : %s %d x %d\n", argv[1], pic[0].width, pic[0].height);
    printf("Máscara : %s %d x %d\n", argv[2], pic[1].width, pic[0].height);
    sel = 0; // pic1

    // Define a janela de visualizacao 2D
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, width, height, 0.0);
    glMatrixMode(GL_MODELVIEW);

    // Aloca memória para a imagem de saída
    pic[2].img = malloc(pic[1].width * pic[1].height * 3); // W x H x 3 bytes (RGB)
    // Pinta a imagem resultante de preto!
    memset(pic[2].img, 0, width * height * 3);

    // Cria textura para a imagem de saída
    tex[2] = SOIL_create_OGL_texture((unsigned char *)pic[2].img, pic[2].width, pic[2].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Entra no loop de eventos, não retorna
    glutMainLoop();
}

// Gerencia eventos de teclado
void keyboard(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        // ESC: libera memória e finaliza
        freemem();
        exit(1);
    }
    if (key >= '1' && key <= '3')
        // 1-3: seleciona a imagem correspondente (origem, máscara e resultado)
        sel = key - '1';
    if (key == 's')
    {
        seamcarve(targetW);
    }
    glutPostRedisplay();
}

void arrow_keys(int a_keys, int x, int y)
{
    switch (a_keys)
    {
    case GLUT_KEY_RIGHT:
        if (targetW <= pic[2].width - 10)
            targetW += 1;
        seamcarve(targetW);
        break;
    case GLUT_KEY_LEFT:
        if (targetW > 10)
            targetW -= 1;
        seamcarve(targetW);
        break;
    default:
        break;
    }
}
// Faz upload da imagem para a textura,
// de forma a exibi-la na tela
void uploadTexture()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 target->width, target->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, target->img);
    glDisable(GL_TEXTURE_2D);
}

// Callback de redesenho da tela
void draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Preto
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Para outras cores, veja exemplos em /etc/X11/rgb.txt

    glColor3ub(255, 255, 255); // branco

    // Ativa a textura corresponde à imagem desejada
    glBindTexture(GL_TEXTURE_2D, tex[sel]);
    // E desenha um retângulo que ocupa toda a tela
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2f(0, 0);

    glTexCoord2f(1, 0);
    glVertex2f(pic[sel].width, 0);

    glTexCoord2f(1, 1);
    glVertex2f(pic[sel].width, pic[sel].height);

    glTexCoord2f(0, 1);
    glVertex2f(0, pic[sel].height);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Exibe a imagem
    glutSwapBuffers();
}