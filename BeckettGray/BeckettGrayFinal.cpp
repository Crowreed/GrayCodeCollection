#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "EvalPerf.hpp"
#include <bitset>
using namespace std;

int n; //Nombre de bits du code de Beckett Gray
vector<int> bgc; //Stockage du code de Beckett Gray courant
int s; //Longueur courante de bgc
int n2; //n**2
int smax; //La plus grande taille de bgc jusqu'à présent
vector<bool> avail; //Vecteur pour savoir quel entiers binaires on déjà été explorés
vector<vector<int>> listeCodeBGC; //Vecteur contenant tous les codes de Beckett Gray générés
vector<vector<int>> bi; //Vectuer des entiers < à n**2 en binaire
vector<vector<int>> flip; //flip[i][j] donne l'entier que l'on obtient si on flips le j-ème bit du nombre i
vector<int> old; //Vecteur sur l'ordre d'entrée des 1 dans le code de Beckett Gray
vector<int> oldS; //Vecteur sur la position d'entrée (s) des 1 dans le code de Beckett Gray
vector<int> nombreVoisins; //Donne le nombre de voisins restant pour chaque sommet dans l'hypercube correspondant à un nombre
EvalPerf PE; //Evaluation de performances de l'algo
vector<int> nbMax; //Nombre maximum de 1 d'affilés autorisés dans le code de Beckett Gray
int nbAppel; //Nombre d'appels de la fonction BGCHeuristique
int bmin; //Nombre minimal du plus grand nombre de 1 dans tous les codes de Beckett Gray.
int bmax; //Nombre maximal du plus grand nombre de 1 dans tous les codes de Beckett Gray.
vector<int> V; //l1[i] donne le nombre de 1 dans la représentation binaire de i
vector<int> nbEltEns; //Nombre d'éléments restant dans un ensemble des nombres avec le même nombre de 1



vector<int> Binary(int x, int n) { 
    //Renvoie la forme binaire d'un entier x sur n bits
    string b=bitset<32>(static_cast<unsigned long long>(x)).to_string(); 
    vector<int> res;
    for (int i=b.size()-n; i<b.size(); i++) {
        res.push_back(b[i]-'0');
    }
    return res;
}

vector<int> listeDes1(int n) {
    //Constructeur de V
    vector<int> res;
    for (int i=0; i<pow(2,n); i++) {
        int cmpt=0;
        int nb=i;
        while (nb>0) {
            cmpt+=nb & 1;
            nb >>= 1;
        }
        res.push_back(cmpt);
        nbEltEns[cmpt]++;
    }
    return res;
}

vector<vector<int>> listeBinaire(int n) {
    //Liste de la forme de tous les entiers sur n bits, constructuer de bi
    vector<vector<int>> res;
    for (int i=0; i<pow(2,n); i++) {
        res.push_back(Binary(i,n));
    }
    return res;
}

bool pw2(int n) {
    //Test pour savoir si un entier est une puissance de 2
    return n!=0 && (n & (n-1))==0;
}



vector<vector<int>> listeFlip(int n) {
    //Retourne un vecteur qui donne tous les entiers que l'on obitent si on inverse un seul bit d'un nombre, constructeur de flip
    vector<vector<int>> res;
    for (int i=0; i<pow(2,n); i++) {
        res.push_back(vector<int>());
        for (int j=0; j<n; j++) {
            res[i].push_back(i^(1<<j));// XOR entre le jème bit et 1 qui flip le bit
        }
    }
    return res;
}



void toStringBool(vector<bool> l){
    //Affichage des vecteurs de booléens
    for (bool b: l){
        cout<<b<< " ";
    }
    cout<<endl;
}

void toStringInt(vector<int> l){
    //Affichage des vecteurs d'entiers
    for (int i: l){
        cout<<i<< " ";
    }
    cout<<endl;
}

void compteLongeur1(int i){
    //Retourne dans tous les codes de gray celui qui a le plus grand nombre de 1 d'affilé, et celui qui en a le plus petit nombre.
    int max=0;
    for (int k=0; k<n; k++){
        int cmpt=0;
        for (int j=0; j<n2; j++){
            if (bi[listeCodeBGC[i][j]][k]==1){
                cmpt++;
            } else {
                if (cmpt>max){
                    max=cmpt;
                }
                cmpt=0;
            }
        }
    }
    if (max>bmax){
        cout<<i<<"max : "<<max<<endl;
        toStringInt(listeCodeBGC[i]);
        bmax=max;
    };
    if (max<bmin){
        cout<<i<<"min : "<<max<<endl;
        toStringInt(listeCodeBGC[i]);
        bmin=max;
    }
}

void BGC(int x) {
    //Retourne tous les code de Beckett Gray sur n bits, x l'entier du code de Beckett Gray courant
    nbAppel++;
    if (s >= n2) {
        if (s>smax){
            smax++;
        }
        //Si on a généré le code de Beckett Gray en entier
        listeCodeBGC.push_back(bgc);
        //On stocke dans listeCodeBGC et dans un fichier texte
        PE.stop();
        std::ofstream tmp("time.txt", std::ios::app);
        tmp << "\n"<<PE.seconds();
        tmp.close();
        std::ofstream otp("codes.txt", std::ios::app);
        otp << "\n";
        for (int i : bgc) {
            otp<<i;
            otp<<" ";
        }
        otp.close();
        return;
    }
    if (s>=smax){
        //On regarde la plus grande suite d'entiers de bgc pour l'instant générée
        if (s>smax){
            smax++;
        }
        PE.stop();
        std::ofstream tmp("time.txt", std::ios::app);
        tmp <<"\n"<<smax<<"\n"<< PE.seconds();
        tmp.close();
        //On stocke dans un fichier texte
        std::ofstream otp("codes.txt", std::ios::app);
        otp << "\n"<<smax<<"\n";
        for (int i : bgc) {
            otp<<i;
            otp<<" ";
        }
        otp.close();
    }
    if (s>1 && s-oldS[0]>nbMax[n]){
        //Si on essaye de générer un code de Beckett Gray avec plus de 1 d'affilé que le maximum possible
        return;
    }
    if (V[x]>1 and nbEltEns[V[x]-1]==0){
        return;
    }
    int cmpt=0;//Nombre de sommets isolés
    int xn=0;//Un sommet que l'on obtient en flippant un bit de x
    int bit=0;//Le bit qui change entre x et xn
    for (int i=0; i<n; i++) {
        int fxi=flip[x][i];
        if (nombreVoisins[fxi]==1 && avail[fxi]){
            //Si on a un voisins de x de degré 1
            cmpt++;
            if (cmpt>1){
                //Si on a plus de 1 sommet isolé il n'y a plus de chemin hamiltonien
                return;
            }
            xn=fxi;
            bit=n-i-1;
        }
    }
    if (cmpt==1 && (bi[xn][bit]==1 || bit==old[0])){
        //Si on a un sommet isolé voisin de x
        int oldS0=oldS[0];
        if (bi[xn][bit]==1){
            //Si on passe d'un 0 à un 1, on ajoute bit dans l'odre d'entrée des 1 dans le code
            old.push_back(bit);
            oldS.push_back(s);
        }
        else{
            //Si on passe d'un 1 à un 0, on retire le premier élément de old, le deuxième bit de old devient le 1 rentré il y a le plus de temps
            old.erase(old.begin());
            oldS.erase(oldS.begin());
        }
        bgc.push_back(xn); //On ajoute xn au code de Beckett Gray
        for (int j=0; j<n; j++) {
            //On baisse le compteur du nombre de voisins de tous les voisins de xn
            nombreVoisins[flip[xn][j]]--;
        }
        avail[xn]=false;
        nbEltEns[V[xn]]--;
        s++; //On augmente la taille de bgc de 1
        BGC(xn); //On peut explorer les voisins de xn
        //On annule tous les changements fait précédemment pour pouvoir explorer d'autres solutions.
        nbEltEns[V[xn]]++;
        s--;
        for (int j=0; j<n; j++) {
            nombreVoisins[flip[xn][j]]++;
        }
        if (bi[xn][bit]==1){
            old.pop_back();
            oldS.pop_back();
        }
        else{
            old.insert(old.begin(),bit);
            oldS.insert(oldS.begin(),oldS0);
        }
        avail[xn]=true;
        bgc.pop_back();
        return;
    }
    if (cmpt==0) {
        //Si on a pas de sommet isolé
        for (int i=0; i<n; i++) {
            //On parcours tous les voisins de x
            xn=flip[x][i];
            bit=n-i-1;
            if (avail[xn] && (bi[xn][bit]==1 || bit==old[0])) {
                //Le fonctionnement à identique à ci-dessus pour le cas du voisin isolé
                int oldS0=(oldS.empty() ? 0 : oldS[0]);
                if (bi[xn][bit]==1){
                    old.push_back(bit);
                    oldS.push_back(s);
                }
                else{
                    old.erase(old.begin());
                    oldS.erase(oldS.begin());
                }
                bgc.push_back(xn);
                for (int j=0; j<n; j++) {
                    nombreVoisins[flip[xn][j]]--;
                }
                s++;
                avail[xn]=false;
                nbEltEns[V[xn]]--;
                BGC(xn);
                nbEltEns[V[xn]]++;
                s--;
                for (int j=0; j<n; j++) {
                    nombreVoisins[flip[xn][j]]++;
                }
                if (bi[xn][bit]==1){
                    old.pop_back();
                    oldS.pop_back();
                }
                else{
                    old.insert(old.begin(),bit);
                    oldS.insert(oldS.begin(),oldS0);
                }
                avail[xn]=true;
                bgc.pop_back();
            }
        }
        return;
    }
return;
}



int main() {
    nbMax={0,1,2,8,16,7,10,15,20};
    n=5;
    nbAppel=0;
    n2=pow(2,n);
    smax=0;
    bgc={0};
    listeCodeBGC={};
    avail.push_back(false);
    for (int i=0; i<n2-1; i++){
        avail.push_back(true);
    }
    for (int i=0; i<n2; i++){
        nombreVoisins.push_back(n);
    }
    bi=listeBinaire(n);
    flip=listeFlip(n);
    s=1;
    for (int i=0; i<n+1; i++){
        nbEltEns.push_back(0);
    }
    old={};
    oldS={};
    bmax=0;
    bmin=100000;
    V=listeDes1(n);
    PE.start();
    BGC(0);
    PE.stop();
    std::cout << " nbr cycles : " << PE.cycles() << std::endl;
    std::cout << " nbr secondes : " << PE.seconds() << std::endl;
    std::cout << " nbr millisecondes : " << PE.milliseconds() << std::endl;
    cout << "Total codes: " << listeCodeBGC.size() << endl;
    cout << "Nombres d'appels: " << nbAppel << endl;
    PE.clear();
    return 0;
}