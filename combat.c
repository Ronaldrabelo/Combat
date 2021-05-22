#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h>




const float FPS = 100;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const float THETA = M_PI/4; 
const int RAIO_CAMPO_FORCA = 38;

const float VEL_TANQUE = 1.25;
const float PASSO_ANGULO = M_PI/90;

const float RAIO_TIRO = 8;




typedef struct Ponto
{
	float x, y;
}Ponto;

typedef struct Circulo
{
	Ponto centro;
	float raio;
}Circulo;

typedef struct Retangulo
{
	Ponto sup_esq, inf_dir;
	Ponto centro_ret;
}Retangulo;

typedef struct Tanque
{
	Ponto centro;
	Ponto A, B, C;
	ALLEGRO_COLOR cor;

	float vel;
	float angulo;
	float x_comp, y_comp;
	float vel_angular;

} Tanque;


typedef struct Estatico
{
	Ponto p_estatico;
	float angulacao_x;
	float angulacao_y;
}Estatico;



void desenhacenario(int i, int j){

	
	al_clear_to_color(al_map_rgb(255-j, 255-i, 255-i));

}

void initTanque1(Tanque *t){
	srand(time(NULL));

	t->centro.x = RAIO_CAMPO_FORCA;
	t->centro.y = RAIO_CAMPO_FORCA;
	t->cor = al_map_rgb(rand()%256, rand()%256, rand()%256);

	t->A.y = RAIO_CAMPO_FORCA;
	t->A.x = 0;

	int ax = t->A.x;
	int ay = t->A.y;



	float alpha = M_PI/2 - THETA;
	float h = RAIO_CAMPO_FORCA*sin(alpha);
	float w = RAIO_CAMPO_FORCA*sin(THETA);

	t->B.x = w;
	t->B.y = -h;

	t->C.x = -w;
	t->C.y = -h;

	t->vel = 0;
	t->angulo = -M_PI/2;
	t->x_comp = cos(t->angulo);
	t->y_comp = sin(t->angulo);

	t->vel_angular = 0;

}
void initTanque2(Tanque *t){

	t->centro.x = SCREEN_W-RAIO_CAMPO_FORCA;
	t->centro.y = SCREEN_H-RAIO_CAMPO_FORCA;
	t->cor = al_map_rgb(rand()%256, rand()%256, rand()%256);

	t->A.y = -RAIO_CAMPO_FORCA;
	t->A.x = 0;

	int ax = t->A.x;
	int ay = t->A.y;

	float alpha = M_PI/2 - THETA;
	float h = RAIO_CAMPO_FORCA*sin(alpha);
	float w = RAIO_CAMPO_FORCA*sin(THETA);

	t->B.x = -w;
	t->B.y = h;

	t->C.x = w;
	t->C.y = h;

	t->vel = 0;
	t->angulo = M_PI/2;
	t->x_comp = cos(t->angulo);
	t->y_comp = sin(t->angulo);

	t->vel_angular = 0;

}

void initRetangulo(Retangulo *ret){

	float a=0.0,b=0.0,c=0.0,d=0.0;


	a = (rand()%((SCREEN_W-(2*RAIO_CAMPO_FORCA+1))) + (2*RAIO_CAMPO_FORCA));
	b = (rand()%((SCREEN_H-(2*RAIO_CAMPO_FORCA+1))) + (2*RAIO_CAMPO_FORCA));
	c = (rand()%((SCREEN_W-(2*RAIO_CAMPO_FORCA+1))) + (2*RAIO_CAMPO_FORCA));
	d = (rand()%((SCREEN_H-(2*RAIO_CAMPO_FORCA+1))) + (2*RAIO_CAMPO_FORCA));

	if(a<=c){
		ret->sup_esq.x = a;
		ret->inf_dir.x = c;
	}
	else{
		ret->sup_esq.x = c;
		ret->inf_dir.x = a;
	}

	if(b<=d){
		ret->sup_esq.y = b;
		ret->inf_dir.y = d;
	}
	else{
		ret->sup_esq.y = d;
		ret->inf_dir.y = b;
	}

	ret->centro_ret.x = (ret->inf_dir.x + ret->sup_esq.x)/2;
	ret->centro_ret.y = (ret->inf_dir.y + ret->sup_esq.y)/2;
	}



void desenhaTanque(Tanque t){
	al_draw_circle(t.centro.x, t.centro.y, RAIO_CAMPO_FORCA, t.cor,1);
	
	al_draw_filled_triangle(t.A.x + t.centro.x, t.A.y+t.centro.y,
		t.B.x + t.centro.x, t.B.y + t.centro.y,
		t.C.x + t.centro.x, t.C.y + t.centro.y, t.cor);
}

int desenhaTiro(Estatico est, int vel, int *val, Tanque t){

	float px = 0;
	float py = 0;
	int valor = *val;

	if (valor>=1)
	{
	px = est.p_estatico.x;
	py = est.p_estatico.y;
	al_draw_filled_circle(px - vel*est.angulacao_x, py - vel*est.angulacao_y, RAIO_TIRO, al_map_rgb(200,200,0));
	return 1;
	}
	else{
	px = t.A.x + t.centro.x;
	py = t.A.y + t.centro.y;
	al_draw_filled_circle(px - t.x_comp, py - t.y_comp, RAIO_TIRO, al_map_rgb(200,200,0));	
	return 0;
	valor=0;
	}
}

void desenhaObstaculo(Retangulo ret){

	al_draw_filled_rectangle(ret.sup_esq.x, ret.sup_esq.y , ret.inf_dir.x, ret.inf_dir.y, al_map_rgb(0,200,100));

}


void Rotate(Ponto *P, float Angle){
	float x=P->x, y=P->y;

	P->x=(x*cos(Angle))-(y*sin(Angle));
	P->y=(y*cos(Angle))+(x*sin(Angle));
}

void rotacionaTanque(Tanque *t){

	if(t->vel_angular!=0){

	Rotate(&t->A, t->vel_angular);
	Rotate(&t->B, t->vel_angular);
	Rotate(&t->C, t->vel_angular);		

	t->angulo += t->vel_angular;
	t->x_comp = cos(t->angulo);
	t->y_comp = sin(t->angulo);


	}
}

float distancia (Ponto p1, Ponto p2){
	return sqrt(pow(p1.x-p2.x,2) + pow(p1.y-p2.y,2));
}

float distancia2 (float x1, float y1, float x2, float y2){
	return sqrt(pow(x1-x2,2) + pow(y1-y2,2));
}

int colisaoCirculoRetangulo(Tanque t, Retangulo ret){
	if (t.centro.x >= ret.sup_esq.x && t.centro.x <= ret.inf_dir.x) {
		if(t.centro.y + RAIO_CAMPO_FORCA >= ret.sup_esq.y && t.centro.y - RAIO_CAMPO_FORCA <= ret.inf_dir.y){
			return 1;
		}
		else{
			return 0;
		}
	}

	if (t.centro.y >= ret.sup_esq.y && t.centro.y <= ret.inf_dir.y) {
		if(t.centro.x + RAIO_CAMPO_FORCA >= ret.sup_esq.x && t.centro.x - RAIO_CAMPO_FORCA <= ret.inf_dir.x){
			return 1;
		}
		else{
			return 0;
				}
	}

	if (distancia(t.centro, ret.inf_dir) <= RAIO_CAMPO_FORCA||
		distancia(t.centro, ret.sup_esq) <= RAIO_CAMPO_FORCA||
		distancia2(t.centro.x, t.centro.y, ret.sup_esq.x, ret.inf_dir.y)<=RAIO_CAMPO_FORCA||
		distancia2(t.centro.x, t.centro.y, ret.inf_dir.x, ret.sup_esq.y)<=RAIO_CAMPO_FORCA)
		return 1;

	else
	{
		return 0;
	}
}

int colisaoTiroRetangulo(Estatico est, int vel, int *val, Tanque tanqueAtirador, Tanque tanqueMorto, Retangulo ret){
		
		int px = est.p_estatico.x;
		int py = est.p_estatico.y;

	if (px - vel*est.angulacao_x >= ret.sup_esq.x && px - vel*est.angulacao_x <= ret.inf_dir.x) {
		if(py - vel*est.angulacao_y >= ret.sup_esq.y && py - vel*est.angulacao_y  <= ret.inf_dir.y){
			return 1;
		}
		else{
			return 0;
		}
	}

	if (py - vel*est.angulacao_y >= ret.sup_esq.y && py - vel*est.angulacao_y <= ret.inf_dir.y) {
		if(px - vel*est.angulacao_x  >= ret.sup_esq.x && px - vel*est.angulacao_x  <= ret.inf_dir.x){
			
			return 1;
		}
		else{
			return 0;
				}
	}

	if (distancia2(px - vel*est.angulacao_x, py - vel*est.angulacao_y, ret.inf_dir.x, ret.inf_dir.y)<= RAIO_TIRO||
		distancia2(px - vel*est.angulacao_x, py - vel*est.angulacao_y, ret.sup_esq.x, ret.sup_esq.y)<= RAIO_TIRO||
		distancia2(px - vel*est.angulacao_x, py - vel*est.angulacao_y, ret.sup_esq.x, ret.inf_dir.y)<= RAIO_TIRO||
		distancia2(px - vel*est.angulacao_x, py - vel*est.angulacao_y, ret.inf_dir.x, ret.sup_esq.y)<= RAIO_TIRO)
		return 1;

	else
	{
		return 0;
	}


}

int batidaEntreTanques(Tanque t1, Tanque t2){

	int centro_t1_x = t1.centro.x;
	int centro_t2_x = t2.centro.x;
	int centro_t1_y = t1.centro.y;
	int centro_t2_y = t2.centro.y;
	int dist_entre_centros = (centro_t1_x - centro_t2_x)*(centro_t1_x - centro_t2_x) + (centro_t1_y - centro_t2_y)*(centro_t1_y - centro_t2_y);
	int raio_ao_quadrado = (RAIO_CAMPO_FORCA+RAIO_CAMPO_FORCA)*(RAIO_CAMPO_FORCA+RAIO_CAMPO_FORCA);
	if (dist_entre_centros <= raio_ao_quadrado){
		return 1;
	}
	else {
		return 0;
	}
}



int batidaTanqueTiro(Estatico est, int vel, int *val, Tanque tanqueMorto){
		int px = est.p_estatico.x;
		int py = est.p_estatico.y;

		int centro_tanque_x = tanqueMorto.centro.x;
		int centro_tanque_y = tanqueMorto.centro.y;

		int dist_entre_centros = ((px - vel*est.angulacao_x) - centro_tanque_x)*((px - vel*est.angulacao_x) - centro_tanque_x) + ((py - vel*est.angulacao_y) - centro_tanque_y)*((py - vel*est.angulacao_y) - centro_tanque_y);

		int raio_ao_quadrado = (RAIO_CAMPO_FORCA+RAIO_TIRO)*(RAIO_CAMPO_FORCA+RAIO_TIRO);

		if (dist_entre_centros <= raio_ao_quadrado){
		
		
		return 1;
		
	}
	else {
		return 0;
	}


}

int batidaTiroTiro(Estatico est, Estatico est1 ,int vel1, int vel2){
		int px = est.p_estatico.x;
		int py = est.p_estatico.y;

		int px1 = est1.p_estatico.x;
		int py1 = est1.p_estatico.y;

		int dist_entre_centros = ((px - vel1*est.angulacao_x) - (px1 - vel2*est1.angulacao_x))*((px - vel1*est.angulacao_x) - (px1 - vel2*est1.angulacao_x)) + ((py - vel1*est.angulacao_y) - (py1 - vel2*est1.angulacao_y))*((py - vel1*est.angulacao_y) - (py1 - vel2*est1.angulacao_y));

		int raio_ao_quadrado = (RAIO_TIRO+RAIO_TIRO)*(RAIO_TIRO+RAIO_TIRO);

		if (dist_entre_centros <= raio_ao_quadrado){
		
		
		return 1;
		
	}
	else {
		return 0;
	}


}

int colisaoTiroComBorda(Estatico est, int vel, int *val, Tanque tanqueAtirador, Tanque tanqueMorto){
	//esquema de colisões do tiro com a borda da tela:
	int px = est.p_estatico.x;
	int py = est.p_estatico.y;


	if(py - vel*est.angulacao_y > RAIO_TIRO&&
		py - vel*est.angulacao_y < -RAIO_TIRO + SCREEN_H&&
		px - vel*est.angulacao_x > RAIO_TIRO&&
		px - vel*est.angulacao_x < -RAIO_TIRO + SCREEN_W){
	return 0;
	}
	else{
		return 1;
	}
}


void colisaoComBorda(Tanque *t){
	//esquema de colisões com a borda da tela:
	if(t->centro.y > RAIO_CAMPO_FORCA ){
	t->centro.y += t->vel*t->y_comp;
	}
	else{
		t->centro.y += VEL_TANQUE;
	}
	if (t->centro.y < -RAIO_CAMPO_FORCA + SCREEN_H){	
	t->centro.y += t->vel*t->y_comp;
	}
	else{
		t->centro.y -= VEL_TANQUE;
	}
	if (t->centro.x > RAIO_CAMPO_FORCA){
		t->centro.x += t->vel*t->x_comp;
	}
	else {
		t->centro.x += VEL_TANQUE;
	}
	if (t->centro.x < -RAIO_CAMPO_FORCA + SCREEN_W)
	{
		t->centro.x += t->vel*t->x_comp;
	}
	else{
		t->centro.x -= VEL_TANQUE;
	}
}

void atualizaTanque(Tanque *t){
	rotacionaTanque(t);
	colisaoComBorda(t);
}






int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
   


	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}


	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}

	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}


   


	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(event_queue, al_get_mouse_event_source());  


	al_install_audio();
	al_init_acodec_addon();

	ALLEGRO_SAMPLE *soundEffect = NULL;
	ALLEGRO_SAMPLE *explosao = NULL;
	ALLEGRO_SAMPLE *explosao2 = NULL;	
	soundEffect = al_load_sample("tanquee.wav");
	explosao = al_load_sample("expl.wav");
	explosao2 = al_load_sample("expl2.wav");

	al_reserve_samples(5);

	Tanque tanque_1;
	Tanque tanque_2;
	initTanque1(&tanque_1);
	initTanque2(&tanque_2);

	Retangulo ret;
	initRetangulo(&ret);


	int pontuacao1 = 0;
	int pontuacao2 = 0;

	char pontuacao_t1[3] = " ";
	char pontuacao_t2[3] = " ";

	pontuacao_t1[0] = pontuacao1;
	pontuacao_t2[0] = pontuacao2;

	char hist1[30];
	char hist2[30];
	char histjogo[20];

	int pt1 = 0;
	int pt2 = 0;
	int UltimoJogo = 0;

	FILE *arq = fopen("recorde.txt","r");

	fscanf(arq,"%d %d %d", &pt1, &pt2, &UltimoJogo);
	printf("%d %d %d", pt1, pt2, UltimoJogo);

	fclose(arq);

	int valor_a = 0;
	int valor_b = 0;


	int pause = 3;

	int veltiro1=0;
	int veltiro2=0;

	int *velshot1 = &veltiro1;
	int *velshot2 = &veltiro2;

	int i = 0,j = 0;
	

	Estatico estat1;
	Estatico estat2;

	float my = (ret.inf_dir.y - ret.sup_esq.y)/2;
	float mx = (ret.inf_dir.x - ret.sup_esq.x)/2;


	//inicia o temporizador
	al_start_timer(timer);
	


	int playing = 1;
	
	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);


		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {





			desenhacenario(i, j);



			desenhaObstaculo(ret);
			

			sprintf(pontuacao_t1, "%d", pontuacao1);
			sprintf(pontuacao_t2, "%d", pontuacao2);
			al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W-SCREEN_W/10 , SCREEN_H/8, 0, pontuacao_t1);
			al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/10, SCREEN_H/8, 0, pontuacao_t2);

			desenhaTanque(tanque_1);
			desenhaTanque(tanque_2);



			if (!colisaoCirculoRetangulo(tanque_2, ret)&&(!colisaoCirculoRetangulo(tanque_1, ret)))
			{
				
				atualizaTanque(&tanque_1);
				atualizaTanque(&tanque_2);
				
				

			}
			else{
				//al_play_sample(explosao,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
				if (colisaoCirculoRetangulo(tanque_2, ret)&&(!colisaoCirculoRetangulo(tanque_1, ret)))
				{
					
				
				if(tanque_2.centro.y-ret.sup_esq.y<ret.inf_dir.y-tanque_2.centro.y){
					tanque_2.centro.y--;
					

					
				}
				if(tanque_2.centro.y-ret.sup_esq.y>ret.inf_dir.y-tanque_2.centro.y){
					tanque_2.centro.y++;
					
				}
				if(tanque_2.centro.x-ret.sup_esq.x<ret.inf_dir.x-tanque_2.centro.x){
					tanque_2.centro.x--;
					
				}
				if(tanque_2.centro.x-ret.sup_esq.x>ret.inf_dir.x-tanque_2.centro.x){
					tanque_2.centro.x++;
					
				}
				}

				else{
				if(tanque_1.centro.y-ret.sup_esq.y<ret.inf_dir.y-tanque_1.centro.y){
					tanque_1.centro.y--;
					
				}
				if(tanque_1.centro.y-ret.sup_esq.y>ret.inf_dir.y-tanque_1.centro.y){
					tanque_1.centro.y++;
					
				}
				if(tanque_1.centro.x-ret.sup_esq.x<ret.inf_dir.x-tanque_1.centro.x){
					tanque_1.centro.x--;
					
				}
				if(tanque_1.centro.x-ret.sup_esq.x>ret.inf_dir.x-tanque_1.centro.x){
					tanque_1.centro.x++;
					
				}
			}
		}

			

		
		
		
		
			desenhaTiro(estat1, veltiro1, &valor_a, tanque_1);
			desenhaTiro(estat2, veltiro2, &valor_b, tanque_2);
		
		//o valor de a e de b indicam se há um tiro em d
			if (desenhaTiro(estat1, veltiro1, &valor_a, tanque_1))
			{
				*velshot1+=5;
			}
			if (desenhaTiro(estat2, veltiro2, &valor_b, tanque_2)){
				*velshot2+=5;
			}

			


			if(colisaoTiroRetangulo(estat1, veltiro1, &valor_a, tanque_1, tanque_2, ret)||
			   colisaoTiroComBorda(estat1, veltiro1, &valor_a, tanque_1, tanque_2))
			{
				valor_a=0;
				*velshot1=0;
				estat1.p_estatico.x=tanque_1.A.x + tanque_1.centro.x;
				estat1.p_estatico.y=tanque_1.A.y + tanque_1.centro.y;
				estat1.angulacao_x = tanque_1.x_comp;
				estat1.angulacao_y = tanque_1.y_comp;	
				
			}
			
			if(colisaoTiroRetangulo(estat2, veltiro2, &valor_b, tanque_2, tanque_1, ret)||
			   colisaoTiroComBorda(estat2, veltiro2, &valor_b, tanque_2, tanque_1))
			{
				valor_b=0;
				*velshot2=0;
				estat2.p_estatico.x=tanque_2.A.x + tanque_2.centro.x;
				estat2.p_estatico.y=tanque_2.A.y + tanque_2.centro.y;
				estat2.angulacao_x = tanque_2.x_comp;
				estat2.angulacao_y = tanque_2.y_comp;	
			}

			if(batidaTanqueTiro(estat1, veltiro1, &valor_a, tanque_2))
			{
				valor_a=0;
				*velshot1=0;
				estat1.p_estatico.x=tanque_1.A.x + tanque_1.centro.x;
				estat1.p_estatico.y=tanque_1.A.y + tanque_1.centro.y;
				estat1.angulacao_x = tanque_1.x_comp;
				estat1.angulacao_y = tanque_1.y_comp;
				pontuacao2++;	
				j=j+15;
				printf("pontuacao2: %d\n\n",pontuacao2);

				
			}

			if(batidaTanqueTiro(estat2, veltiro2, &valor_b, tanque_1))
			{
				valor_b=0;
				*velshot2=0;
				estat2.p_estatico.x=tanque_2.A.x + tanque_2.centro.x;
				estat2.p_estatico.y=tanque_2.A.y + tanque_2.centro.y;
				estat2.angulacao_x = tanque_2.x_comp;
				estat2.angulacao_y = tanque_2.y_comp;
				pontuacao1++;
				i=i+15;
				printf("pontuacao1: %d\n\n",pontuacao1);
				
			}

			if (batidaTiroTiro(estat1, estat2, veltiro1, veltiro2)){
				valor_b=0;
				*velshot2=0;
				estat2.p_estatico.x=tanque_2.A.x + tanque_2.centro.x;
				estat2.p_estatico.y=tanque_2.A.y + tanque_2.centro.y;
				estat2.angulacao_x = tanque_2.x_comp;
				estat2.angulacao_y = tanque_2.y_comp;

				valor_a=0;
				*velshot1=0;
				estat1.p_estatico.x=tanque_1.A.x + tanque_1.centro.x;
				estat1.p_estatico.y=tanque_1.A.y + tanque_1.centro.y;
				estat1.angulacao_x = tanque_1.x_comp;
				estat1.angulacao_y = tanque_1.y_comp;
			}
			
			
									
			
			

				

			//batidaEntreTanques(tanque_1, tanque_2);
			if (batidaEntreTanques(tanque_1, tanque_2)){
				al_play_sample(explosao,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
				tanque_1.centro.x = RAIO_CAMPO_FORCA;
				tanque_1.centro.y = RAIO_CAMPO_FORCA;
				tanque_2.centro.x = SCREEN_W-RAIO_CAMPO_FORCA;
				tanque_2.centro.y = SCREEN_H-RAIO_CAMPO_FORCA;
			}


			if (pontuacao2==5){
				al_clear_to_color(al_map_rgb(0,0,0));

				al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W/10, SCREEN_H/4, 0, "Tank 1 Wins");
				pt1++;
				sprintf(hist1, "Tank 1: %i wins", pt1);
				al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W/10, SCREEN_H/2, 0, hist1);
				
				sprintf(histjogo, "Ultimo vencedor: Tanque %i", UltimoJogo);
				al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W/10, SCREEN_H/1.5, 0, histjogo);

				
				al_flip_display();
				al_rest(3);

				arq = fopen("recorde.txt", "w");

				UltimoJogo = 1;
				fprintf(arq, "%d %d %d", pt1, pt2, UltimoJogo);
				
				break;
			}

			if (pontuacao1==5){
				al_clear_to_color(al_map_rgb(0,0,0));

				al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W/10, SCREEN_H/4, 0, "Tank 2 Wins");
				pt2++;
				sprintf(hist2, "Tank 2: %i wins", pt2);
				al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W/10, SCREEN_H/2, 0, hist2);
				
				sprintf(histjogo, "Ultimo vencedor: Tanque %i", UltimoJogo);
				al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W/10, SCREEN_H/1.5, 0, histjogo);

				
				al_flip_display();
				al_rest(3);

				arq = fopen("recorde.txt", "w");

				UltimoJogo = 2;
				fprintf(arq, "%d %d %d", pt1, pt2, UltimoJogo);
				
				break;
			}

			

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			
			  


			if(al_get_timer_count(timer)%(int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));

		}


		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE||ev.keyboard.keycode==ALLEGRO_KEY_ESCAPE) {
			playing = 0;
		}
		//se o tipo de evento for um clique de mouse
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			printf("\nmouse clicado em: %d, %d", ev.mouse.x, ev.mouse.y);
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
				
		

		
			switch(ev.keyboard.keycode){

			

			case ALLEGRO_KEY_W:

					
					al_play_sample(soundEffect,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
					tanque_1.vel -= (VEL_TANQUE);

				break;

				case ALLEGRO_KEY_Q:

					valor_a++;
					if(valor_a==1){
						estat1.p_estatico.x=tanque_1.A.x + tanque_1.centro.x;
						estat1.p_estatico.y=tanque_1.A.y + tanque_1.centro.y;
						estat1.angulacao_x = tanque_1.x_comp;
						estat1.angulacao_y = tanque_1.y_comp;
						al_play_sample(explosao,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);						
						
					}


					
					

				break;

				case ALLEGRO_KEY_ENTER:

					valor_b++;
					if(valor_b==1){
						estat2.p_estatico.x=tanque_2.A.x + tanque_2.centro.x;
						estat2.p_estatico.y=tanque_2.A.y	+ tanque_2.centro.y;
						estat2.angulacao_x = tanque_2.x_comp;
						estat2.angulacao_y = tanque_2.y_comp;						
						al_play_sample(explosao2,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
					}
					

				break;

				case ALLEGRO_KEY_S:

					
					
					tanque_1.vel += VEL_TANQUE;
					

				break;

				case ALLEGRO_KEY_D:

				tanque_1.vel_angular += PASSO_ANGULO;

				break;

				case ALLEGRO_KEY_A:

				tanque_1.vel_angular -= PASSO_ANGULO;

				break;

				case ALLEGRO_KEY_UP:
					
					tanque_2.vel -= (VEL_TANQUE);

				break;

				case ALLEGRO_KEY_DOWN:

					tanque_2.vel += VEL_TANQUE;

				break;

				case ALLEGRO_KEY_RIGHT:

				tanque_2.vel_angular += PASSO_ANGULO;

				break;

				case ALLEGRO_KEY_LEFT:

				tanque_2.vel_angular -= PASSO_ANGULO;

				break;

				case ALLEGRO_KEY_SPACE:

				pause=pause*-1;

				break;

}



		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
		//imprime qual tecla foi
		printf("\ntecla solta: %d", ev.keyboard.keycode);
		

		switch(ev.keyboard.keycode){


				case ALLEGRO_KEY_W:
					
					
					tanque_1.vel += (VEL_TANQUE);

				break;

				case ALLEGRO_KEY_S:


					tanque_1.vel -= VEL_TANQUE;

				break;

				case ALLEGRO_KEY_D:

				tanque_1.vel_angular -= PASSO_ANGULO;

				break;

				case ALLEGRO_KEY_A:

				tanque_1.vel_angular += PASSO_ANGULO;

				break;

				case ALLEGRO_KEY_UP:
					
					tanque_2.vel += (VEL_TANQUE);

				break;

				case ALLEGRO_KEY_DOWN:

					tanque_2.vel -= VEL_TANQUE;

				break;

				case ALLEGRO_KEY_RIGHT:

				tanque_2.vel_angular -= PASSO_ANGULO;

				break;

				case ALLEGRO_KEY_LEFT:

				tanque_2.vel_angular += PASSO_ANGULO;

				break;
}


	}	
	} //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
 	fclose(arq);
	al_destroy_timer(timer);
	al_destroy_sample(explosao);
	al_destroy_sample(explosao2);
	al_destroy_sample(soundEffect);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
   
 
	return 0;
}