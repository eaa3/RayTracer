#include "sdl.h"

SDL :: SDL(string nome){
	string atributo;
	ifstream entrada (nome.c_str());

	while(entrada >> atributo){
		
		if(atributo.compare("eye") == 0){
			entrada >> this->eye.x;
			entrada >> this->eye.y;
			entrada >> this->eye.z;

		}else if(atributo.compare("size") == 0){
			entrada >> this->size[0];
			entrada >> this->size[1];
		
		}else if(atributo.compare("ortho") == 0){
			entrada >> this->ortho[0];
			entrada >> this->ortho[1];
			entrada >> this->ortho[2];
			entrada >> this->ortho[3];
		
		}else if(atributo.compare("ambient") == 0){
			entrada >> this->ambiente;
		
		}else if(atributo.compare("light") == 0){
			Light *l = new Light();
			entrada >> l->dir.x >> l->dir.y >> l->dir.z >> l->Int;
			this->lights.push_back(l);
		
		}else if(atributo.compare("object") == 0){
			Object *ob =  new Object();
			Quad *obj =  new Quad();
			entrada >> obj->a >> obj->b >> obj->c >> obj->d 
					>> obj->e >> obj->f >> obj->g >> obj->h 
				    >> obj->j >> obj->k;
			Mat mat;
			entrada >> mat.r >> mat.g >> mat.b >> mat.Ka >> mat.Kd >> mat.Ks >> mat.n;
			obj->m = mat;
			ob->quad = obj;
			entrada >> ob->KS >> ob->KT >> ob->ir;
			this->objects.push_back(ob);
		
		}else if(atributo.compare("background") == 0){
			entrada >> this->background[0];
			entrada >> this->background[1];
			entrada >> this->background[2];
		
		}else if(atributo.compare("profundidade") == 0){
			entrada >> this->profundidade;
		
		}else if(atributo.compare("output") == 0){
			entrada >> this->output;
		
		}else if(atributo.at(0) == '#'){
			string temp;
			getline(entrada,temp);
		}
	}

	//invertendo o sentido das luzes para ficarem com o sentido real
	for(int i = 0; i < lights.size(); i++) {
		lights.at(i)->dir = svmpy(-1, lights.at(i)->dir);
	}
}