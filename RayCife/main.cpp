#include "quadric.h"
#include "sdl.h"
#include <math.h>

SDL *arq =  new SDL("twoplanessphere-1.sdl");
Vec *tela = new Vec[arq->size[0]*arq->size[1]];

//indica se o raio transmitido é interno ou não a um objeto
bool inside = false;

bool isRaioSombra(Ray ray) {
	bool retorno = false;
	double dist = INT_MAX;
	double distTemp = -1;
	
	for(int i = 0; i < arq->objects.size(); i++) {
		distTemp = intersect(ray, arq->objects.at(i)->quad);
		if(distTemp > 0 && distTemp < dist) {
			dist = distTemp;
		}
	}
	if(dist > 0 && dist < INT_MAX) retorno = true;
	
	return retorno;
}

Vec calcularPonto(Object object, Ray ray){
	Vec retorno;
	Vec temp;
	temp.x = ray.dir.x;
	temp.y = ray.dir.y;
	temp.z = ray.dir.z;
	temp = normalize(temp);

	double dist = intersect(ray, object.quad);
	
	temp = svmpy(dist, temp);
	retorno.x = temp.x + ray.org.x;
	retorno.y = temp.y + ray.org.y;
	retorno.z = temp.z + ray.org.z;

	return retorno;
}	

Vec calcularNormal(Object object, Vec ponto){
	Vec retorno;

	double x = ponto.x;
	double y = ponto.y;
	double z = ponto.z;
	double a = object.quad->a;
	double b = object.quad->b;
	double c = object.quad->c;
	double d = object.quad->d;
	double e = object.quad->e;
	double f = object.quad->f;
	double g = object.quad->g;
	double h = object.quad->h;
	double j = object.quad->j;

	retorno.x = (2*a*x) + (2*d*y) + (2*f*z) + (2*g);
	retorno.y = (2*b*y) + (2*d*x) + (2*e*z) + (2*h);
	retorno.z = (2*c*z) + (2*e*y) + (2*f*x) + (2*j);

	return retorno;
}

Vec* conversorStructPointer(Vec vec) {
	Vec* retorno = new Vec();
	retorno->x = vec.x;
	retorno->y = vec.y;
	retorno->z = vec.z;
	
	return retorno;
}

Vec conversorPointerStruct(Vec *vec) {
	Vec retorno;
	retorno.x = vec->x;
	retorno.y = vec->y;
	retorno.z = vec->z;
	
	return retorno;
}

Ray raioRefletido(Object object, Ray ray){
	Ray retorno;
	
	//achando a normal
	Vec ponto = calcularPonto(object, ray);
	Vec normal = calcularNormal(object, ponto);
	normal = normalize(normal);

	//vetor oposto ao raio
	Vec rIn = ray.dir;
	rIn = svmpy(-1, rIn);
	rIn = normalize(rIn);
	
	//cosseno entre a normal e o raio incidente
	double cos = dot(normal, rIn);

	Vec temp = normal;
	temp = svmpy(2*cos, temp);
	temp = vsub(temp, rIn);
	temp = normalize(temp);
	
	retorno.dir.x = temp.x;
	retorno.dir.y = temp.y;
	retorno.dir.z = temp.z;
	retorno.org = ponto;
	retorno.depth = ray.depth + 1;

	return retorno;
}


Ray raioTransmitido(Object object, Ray ray){
	Ray retorno;
		
	//achando a normal
	Vec ponto = calcularPonto(object, ray);
	Vec normal = calcularNormal(object, ponto);
	normal = normalize(normal);

	//vetor oposto ao raio
	Vec rIn = ray.dir;
	Vec temp = svmpy(-1, rIn);
	rIn = temp;
	rIn = normalize(rIn);

	//vetor eixo
	Vec eixo = multVetorial(rIn, normal);

	//cos1, sen1, sen2, cos2 e angulo2
	double cos1 = dot(normal, rIn);
	double sen1 = sqrt(1 - pow(cos1, 2));
	double n1 = 0;
	double n2 = 0;
	if(!inside) {
		n1 = 1;
		n2 = object.ir;
		inside = true;
	} else {
		n1 = object.ir;
		n2 = 1;
		inside = false;
	}
	double sen2 = (sen1*n2)/n1;
	double cos2 = sqrt(1 - pow(sen2, 2));
	
	retorno.dir = svmpy(-1, normal);
	retorno.dir.x = ((eixo.x*eixo.x*(1-cos2)+ cos2)*retorno.dir.x) + 
					((eixo.z*eixo.y*(1-cos2)-(eixo.z*sen2))*retorno.dir.y) +
					((eixo.x*eixo.z*(1-cos2)+(eixo.y*sen2))*retorno.dir.z); 

	retorno.dir.y = ((eixo.y*eixo.x*(1-cos2)+(eixo.z*sen2))*retorno.dir.x) + 
					((eixo.z*eixo.x*(1-cos2)+ cos2)*retorno.dir.y) +
					((eixo.y*eixo.z*(1-cos2)-(eixo.x*sen2))*retorno.dir.z); 

	retorno.dir.z = ((eixo.z*eixo.x*(1-cos2)-(eixo.y*sen2))*retorno.dir.x) + 
					((eixo.z*eixo.x*(1-cos2)-(eixo.x*sen2))*retorno.dir.y) +
					((eixo.z*eixo.z*(1-cos2)+ cos2)*retorno.dir.z);

	retorno.org = ponto;
	retorno.depth = ray.depth + 1;

	return retorno;
}

double calcularIntensidadeAmbiente(Object object) {
	double retorno = object.quad->m.Ka * arq->ambiente;
	return retorno;
}


double calcularIntensidadeDifusa(Object object, Ray rayOlho, Light luz) {
	double retorno = 0;
	//achando a normal
	Vec ponto = calcularPonto(object, rayOlho);
	Vec normal = calcularNormal(object, ponto);
	normal = normalize(normal);

	//vetor oposto à luz
	Vec rIn = luz.dir;
	rIn = svmpy(-1, rIn);
	rIn = normalize(rIn);

	//se não for raio de sombra
	Ray raySombra;
	raySombra.dir = rIn;
	raySombra.org = ponto;
	if(!isRaioSombra(raySombra)) {
		
		//intensidade = I * kd * cos
		double cos = dot(normal, rIn);
		if(cos < 0) cos = 0;
		retorno = luz.Int * (object.quad->m.Kd * cos);
	}

	return retorno;
}

double calcularIntensidadeEspecular(Object object, Ray rayOlho, Light luz) {
	double retorno = 0;
	//achando a normal
	Vec ponto = calcularPonto(object, rayOlho);
	Vec normal = calcularNormal(object, ponto);
	normal = normalize(normal);

	//achando o vetor oposto à luz
	Vec rIn = luz.dir;
	rIn = svmpy(-1, rIn);
	rIn = normalize(rIn);

	//R = 2N(N.L) - L ou R = N * 2*cos - L
	double cos = dot(normal, rIn);
	if(cos < 0) cos = 0;
	Vec r = vsub(svmpy(2 * cos, normal), rIn);
	r = normalize(r);

	//achando v (vetor do olho)
	Vec v = rayOlho.dir;
	v = svmpy(-1, v);
	v = normalize(v);

	//se não for raio de sombra
	Ray raySombra;
	raySombra.dir = rIn;
	raySombra.org = ponto;
	if(!isRaioSombra(raySombra)) {

		//intensidade = I * ks*cosª
		double cos2 = dot(r, v);
		if(cos2 < 0) cos2 = 0;
		retorno = luz.Int*(object.quad->m.Ks * (pow(cos2, object.quad->m.n)));
	}

	return retorno;
}

Vec pixelParaPonto2d(int i, int j) {
	Vec retorno;
	double larguraPixel;
	if(arq->ortho[2] > arq->ortho[0]) larguraPixel = (arq->ortho[2] - arq->ortho[0]) / arq->size[0];
	else larguraPixel = (arq->ortho[0] - arq->ortho[2]) / arq->size[0];

	double alturaPixel; 
	if(arq->ortho[3] > arq->ortho[1]) alturaPixel = (arq->ortho[3] - arq->ortho[1]) / arq->size[1];
	else alturaPixel = (arq->ortho[1] - arq->ortho[3]) / arq->size[1];

	retorno.x = (arq->ortho[0] + larguraPixel/2) + (j * larguraPixel);
	retorno.y = (arq->ortho[3] - alturaPixel/2)  - (i * alturaPixel);
	retorno.z = 0;
	
	return retorno;
}

void inicializarTela() {
	for(int i = 0; i < arq->size[0];i++){
		for(int j = 0;j < arq->size[1];j++){
			tela[(i*arq->size[1]) + j].x = arq->background[0];
			tela[(i*arq->size[1]) + j].y = arq->background[1];
			tela[(i*arq->size[1]) + j].z = arq->background[2];
		}
	}
}


Vec shade(Object object, Ray ray) {
	Vec retorno;
	retorno.x = 0;
	retorno.y = 0;
	retorno.z = 0;

	double intensidadeAmbiente = calcularIntensidadeAmbiente(object);
	
	double intensidadeDifusa = 0;
	double intensidadeEspecular = 0;
	for(int i = 0; i < arq->lights.size(); i++) {
		intensidadeDifusa += calcularIntensidadeDifusa(object, ray, *arq->lights.at(i));
		intensidadeEspecular += calcularIntensidadeEspecular(object, ray, *arq->lights.at(i));
	}
	
	retorno.x = (object.quad->m.r * (intensidadeAmbiente + intensidadeDifusa)) + intensidadeEspecular;
	retorno.y = (object.quad->m.g * (intensidadeAmbiente + intensidadeDifusa)) + intensidadeEspecular;
	retorno.z = (object.quad->m.b * (intensidadeAmbiente + intensidadeDifusa)) + intensidadeEspecular;

	if(retorno.x > 1) retorno.x = 1;
	if(retorno.y > 1) retorno.y = 1;
	if(retorno.z > 1) retorno.z = 1;

	return retorno;
}

//retorna o índice do objeto mais próximo ao ponto de origem do raio, se não existir retorna -1
int objetoMaisProximo(Ray ray) {
	int retorno = -1;
	double dist = INT_MAX;
	double distTemp = -1;
	
	for(int i = 0; i < arq->objects.size(); i++) {
		distTemp = intersect(ray, arq->objects.at(i)->quad);
		if(distTemp > 0 && distTemp < dist) {
			dist = distTemp;
			retorno = i;
		}
	}
	if(dist == INT_MAX) retorno = -1;
	
	return retorno;
}

//a = cor local; b = cor refletida; c = cor transmitida;
Vec mesclarCores(Object object, Vec a, Vec b, Vec c) {
	Vec retorno;

	retorno.x = ((1 - object.KS - object.KT) * a.x) + (object.KS * b.x) + (object.KT * c.x);
	retorno.y = ((1 - object.KS - object.KT) * a.y) + (object.KS * b.y) + (object.KT * c.y);
	retorno.z = ((1 - object.KS - object.KT) * a.z) + (object.KS * b.z) + (object.KT * c.z);


	return retorno;
}

Vec rayTrace(Ray ray) {
	Vec retorno;
	retorno.x = 0;
	retorno.y = 0;
	retorno.z = 0;

	int indiceObject = objetoMaisProximo(ray);

	//objeto nao encontrado
	if(indiceObject < 0) {
		//para resolver o caso em que os raios que saem do olho (eye) não encontram objetos
		if(ray.depth == 0) {
			retorno.x = arq->background[0];
			retorno.y = arq->background[1];
			retorno.z = arq->background[2];
		}
		//para resolver o caso em que na recursão não se encontra o ponto transmitido ou refletido
		else {
			retorno.x = 0.f;
			retorno.y = 0.f;
			retorno.z = 0.f;
		}
	}
	
	//objeto encontrado
	else {
	
		//cálculo das cores local, refletida e transmitida
		Object object = *arq->objects.at(indiceObject);
		Vec corLocal = shade(object, ray);

		//setando para preto as cores transmitidas e refletidas
		Vec corRefletida;
		corRefletida.x = 0;
		corRefletida.y = 0;
		corRefletida.z = 0;

		Vec corTransmitida;
		corTransmitida.x = 0;
		corTransmitida.y = 0;
		corTransmitida.z = 0;

		//recursao
		if(ray.depth < arq->profundidade) {
			//so executa a recursao caso pretender usar as cores obtidas
			if(object.KS > 0) corRefletida = rayTrace(raioRefletido(object, ray));
			if(object.KT > 0) corTransmitida = rayTrace(raioTransmitido(object, ray));
		}

		//mesclando as cores
		retorno = mesclarCores(object, corLocal, corRefletida, corTransmitida);
	}
	return retorno;
}

void pintarTela() {
	for(int i = 0; i < arq->size[0];i++){
		for(int j = 0;j < arq->size[1];j++){
			//criação do raio que sai do olho e passa pelo ponto relativo ao pixel atual
			Ray ray;
			ray.depth = 0;
			ray.org = arq->eye;
			ray.dir = vsub(pixelParaPonto2d(i, j), arq->eye);

			if(i >= 90 && i <= 110 && j >= 80 && j <= 120) {
				int teste = 0;
			}

			//ray trace no pixel atual retornando sua cor para ser pintada na tela
			tela[(i*arq->size[1]) + j] = rayTrace(ray);
		}
	}
}

void escreverSaida() {
	
	ofstream saida(arq->output.c_str(), ios::out,ios::trunc);
	saida << "P3" << "\n";
	saida << arq->size[0] << " " << arq->size[1] << "\n";
	saida << 255 << "\n";
	for(int i = 0; i < arq->size[0]; i++){
		for(int j = 0; j < arq->size[1]; j++){
			
			double r = (int)(tela[(i*arq->size[1]) + j].x * 255);
			double g = (int)(tela[(i*arq->size[1]) + j].y * 255);
			double b = (int)(tela[(i*arq->size[1]) + j].z * 255);
			
			saida << r << " " << g << " " << b << "\n";
		}
	}
	saida.close();
}

int main(int argc, char *argv[]) {

	//leitura de arquivo
	/*cout << "digite o nome do arquivo a ser lido" << endl;
	string arquivo;
	cin >> arquivo;

	arq =  new SDL(arquivo.c_str());*/
	//arq = new SDL(argv[1]);
	tela = new Vec[arq->size[0]*arq->size[1]];
	
	inicializarTela();
	pintarTela();
	escreverSaida();
	return 0;
}