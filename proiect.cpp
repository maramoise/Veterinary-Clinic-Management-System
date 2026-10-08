#include <iostream>
#include <string>
#include <vector>
#include<list>
#include <algorithm>
#include <stdlib.h>
#include<typeinfo>
using namespace std;

enum Grav
{
    USOR = 1,
    MEDIU = 2,
    GRAV = 3,
    CRITIC = 4
};

class Diagnostic
{
protected:
    string nume;
    Grav severitate;
public:
    Diagnostic(string nume = "", Grav sev = USOR) : nume(nume), severitate(sev){}

    bool necesitaInternare() const
    {
        if (severitate == GRAV) return true;
        return false;
    }

    virtual bool necesitaOperatie() const
    {
        if (severitate == CRITIC) return true;
        return false;
    }

    virtual Diagnostic* clone() const { return new Diagnostic(*this); }

    virtual ~Diagnostic() {}

    string getNume() const
    {
        return nume;
    }

    
    virtual void citire(istream& in)
    {
        int x;
        cout << " Introduceti cat de grava este afectiunea de la 1 la 4: ";
        in >> x;
        
        while (x>4 || x<1)
        {
            cout << " Introduceti o valoare intre 1 si 4!";
            cin >> x;
        }
        severitate = static_cast<Grav>(x);
    }
    
    friend istream& operator>>(istream& in, Diagnostic& d)
    {
        d.citire(in);
        return in;
    }

    friend class Animal;

};

class Gripa: public Diagnostic
{
public:
    Gripa(Grav sev = USOR) : Diagnostic("Gripa", sev){}
    bool necesitaOperatie() const override
    {
        return false;
    }

    Diagnostic* clone() const override { return new Gripa(*this); }

};

class Fractura: public Diagnostic
{
public:
    Fractura(Grav sev = USOR) : Diagnostic("Fractura", sev){}
        bool necesitaOperatie() const override
    {
        if(severitate > MEDIU) return true;
        return false;
    }
    Diagnostic* clone() const override { return new Fractura(*this); }

};

class Obezitate: public Diagnostic
{
public:
    Obezitate(Grav sev = USOR) : Diagnostic("Obezitate", sev){}
    bool necesitaOperatie() const override
    {
        return false;
    }
    Diagnostic* clone() const override { return new Obezitate(*this); }

};

class Dirofilarie: public Diagnostic
{
public:
    Dirofilarie(Grav sev = USOR) : Diagnostic("Dirofilarie", sev){}
    bool necesitaOperatie() const override
    {
        return false;
    }
    Diagnostic* clone() const override { return new Dirofilarie(*this); }

};

class specieNecunoscuta : public exception
{
public:
    const char* what() const noexcept
    {
        return "Specie invalida! Clinica trateaza doar Caine, Pisica sau Hamster.";
    }
};

class VarstaNegativa : public exception
{
public:
    const char* what() const noexcept
    {
        return "Varsta introdusa trebuie sa fie un numar pozitiv! \n Animalul nu a fost inregistrat!";
    }
};


class Animal
{
protected:
    string nume;
    int varsta;
    Diagnostic* afectiune; //relatie de compozitie
    static int nrAnimale;
    
public:
    Animal(const Diagnostic& afectiune, string nume = "", int varsta = 0); //constructor de initializare
    Animal(const Animal&); //declararea constructorului de copiere
    virtual ~Animal() //virtual ca sa se poata mosteni
    {
        delete afectiune; //pentru o relatie de compozitie cu pointer, trb sa stergem odata cu animalul afectiunea
    } //destructor
    
    Animal& operator=(const Animal& c)
    {
        if (this != &c)
        {
            nume = c.nume;
            delete afectiune;
            if (c.afectiune != nullptr)
                afectiune = c.afectiune->clone();
            else afectiune = nullptr;
            varsta = c.varsta;
        }
        return *this;
    }
    
    void citire(istream&);
    virtual void afisare(ostream&) const;

    string getNumeAfectiune() const
    {
        if (this->afectiune != nullptr) return afectiune->getNume();
        else return "Sanatos";
    }

    bool Internare() const
    {
        if (afectiune != nullptr)
        {
            return afectiune->necesitaInternare();
        }
        return false;
    }
    
    friend istream& operator>>(istream&, Animal&); //supraincarcarea operatorului pentru citire
    friend ostream& operator<<(ostream&, const Animal&); //supraincarcarea operatorului pentru afisare
    
    friend int operator+(const Animal& c1, const Animal& c2); //supraincarcarea operatorului pentru adunarea varstelor
    
    bool operator<(const Animal& c) const
    {
        return this->varsta < c.varsta;
    }
    
    static void raportClinica(const vector<Animal*>& animale);

    virtual int getPretConsult() const = 0;
    virtual int getPretInternare() const = 0;
    virtual int calculeazaRiscOperatie() const = 0;

    bool operatie() const
    {
        if (afectiune != nullptr)
        {
            return afectiune->necesitaOperatie();
        }
        return false;
    }

    int pretOperatie () const
    {
            if (calculeazaRiscOperatie() > 6)
            {
                cout<< "Animalul trebuie sa fie internat. Acesta va beneficia de terapie intensiva in cadrul clinicii noastre.";
                return (getPretInternare() + 200);//riscul este prea mare, animalul va fi internat si va primi terapie intensiva
            }
            cout << "Animalul trebuie sa fie operat. In urma operatiei, acesta va fi internat pana la recuperare.";
            return (getPretInternare() + 10000); //costul operatiei + cel al internarii
    }

    string getNume() const
    {
        return nume;
    }

};

class Caine : public Animal
{
private:
    bool cuminte;
public:
    Caine(const Diagnostic& afectiune, string nume, int varsta, bool cuminte) : Animal(afectiune, nume, varsta)
    {
        this->cuminte = cuminte;
    }

    Caine(const Caine& c) : Animal(c), cuminte(c.cuminte) {}


    Caine& operator=(const Caine& c)
    {
        if (this != &c)
        {
            nume = c.nume;
            delete afectiune;
            if (c.afectiune != nullptr)
                afectiune = c.afectiune->clone();
            else afectiune = nullptr;
            varsta = c.varsta;
            cuminte = c.cuminte;
        }

        return *this;
    }

    int getPretConsult () const override
    {
        return 80;
    }
    int getPretInternare() const override
    {
        return 400;
    }

    int calculeazaRiscOperatie() const override
    {
        int risc = 2;
        if (varsta > 8) risc += 4;
        return risc;
    }

    int taxaBotnita() const
    {
        if (!cuminte)
        {
            cout << "Cainelui i s-a pus botnita. Taxa: 30 RON";
            return 30;
        }
        return 0;
    }

};

class Pisica : public Animal
{
public:
    Pisica(const Diagnostic& afectiune, string nume, int varsta) : Animal(afectiune, nume, varsta) {}

    Pisica(const Pisica& c) : Animal(c) {}

    int getPretConsult () const override
    {
        return 80;
    }
    int getPretInternare() const override
    {
        return 350;
    }

    int calculeazaRiscOperatie() const override
    {
        int risc = 3;
        if (varsta > 8) risc += 3;
        return risc;
    }
    Pisica& operator=(const Pisica& c)
    {
        if (this != &c)
        {
            nume = c.nume;
            delete afectiune;
            if (c.afectiune != nullptr)
                afectiune = c.afectiune->clone();
            else afectiune = nullptr;
            varsta = c.varsta;
        }

        return *this;
    }
};

class Hamster : public Animal
{
public:
    Hamster(const Diagnostic& afectiune, string nume, int varsta) : Animal(afectiune, nume, varsta) {}
    Hamster& operator=(const Hamster& c)
    {
        if (this != &c)
        {
            nume = c.nume;
            delete afectiune;
            if (c.afectiune != nullptr)
                afectiune = c.afectiune->clone();
            else afectiune = nullptr;
            varsta = c.varsta;
        }

        return *this;
    }

    Hamster(const Hamster& c) : Animal(c) {}

    int getPretConsult () const override
    {
        return 50;
    }
    int getPretInternare() const override
    {
        return 250;
    }

    int calculeazaRiscOperatie() const override
    {
        int risc = 6;
        if (varsta > 2) risc += 4;
        return risc;
    }
};
void Animal::raportClinica(const vector<Animal*>& animale)

    {
    if (nrAnimale == 0)
    {
        cout << "Clinica este goala!\n";
        return;
    }

    int nrCaini = 0, nrPisici = 0, nrHamsteri = 0;
    int nrInternati = 0, nrOperati = 0;

    for (const Animal* a : animale)
    {
        if (dynamic_cast<const Caine*>(a)) nrCaini++;
        else if (dynamic_cast<const Pisica*>(a)) nrPisici++;
        else if (dynamic_cast<const Hamster*>(a)) nrHamsteri++;

        if (a->Internare()) nrInternati++;
        if (a->operatie()) nrOperati++;
    }

    cout << "RAPORT CLINICA: \n";
    cout << "Total animale: " << nrAnimale << endl;
    cout << "Caini: " << nrCaini << ", Pisici: " << nrPisici << ", Hamsteri: " << nrHamsteri << endl;
    cout << "Necesita internare: " << nrInternati << endl;
    cout << "Necesita operatie: " << nrOperati << endl;
}

class Proprietar
{
private:
    string nume; //numele de familie al proprietarului
    vector<Animal*> animale; //animalele acestuia, relatie de agregare ptc pot exista independent de el
public:
    Proprietar(vector<Animal*> animale, string nume = "") : animale(animale), nume(nume) {}; //constructor de initializare

//cat a cheltuit pe consulturile tuturor animalelor care au fost in trecut la aceasta clinica:

void plataTotala()
{
    int total = 0;

    for (int i = 0; i < animale.size(); i++)
    {
        int costAnimal = 0;
        cout << "\nAnimal: " << animale[i]->getNume();

        int pretConsult = animale[i]->getPretConsult();
        costAnimal += pretConsult;
        cout << "\n  Consult: " << pretConsult << " RON";

        if (animale[i]->operatie())
        {
            int pretOp = animale[i]->pretOperatie();
            costAnimal += pretOp;
            cout << "\n  Operatie/Terapie intensiva + Internare: " << pretOp << " RON";
        }
        else if (animale[i]->Internare())
        {
            int pretInt = animale[i]->getPretInternare();
            costAnimal += pretInt;
            cout << "\n  Internare: " << pretInt << " RON";
        }

        Caine* c = dynamic_cast<Caine*>(animale[i]); // downcast
        if (c != nullptr)
        {
            int taxa = c->taxaBotnita();
            costAnimal += taxa;
        }

        cout << "\n  Total animal: " << costAnimal << " RON";
        total += costAnimal;
    }

    cout << "\n\nTOTAL DE PLATA: " << total << " RON\n";
}

//functie de adugare animal in lista de animale a proprietarului:

    void adaugaAnimal(Animal& c) {
        animale.push_back(&c);
    }

    string getNume() const { return nume; }

    friend ostream& operator<<(ostream& out, const Proprietar& p)
    {
        out << "Proprietar: " << p.nume << " (Are " << p.animale.size() << " animale inregistrate)\n";
        return out;
    }

    friend istream& operator>>(istream& in, Proprietar& p)
    {
        cout << "Introduceti numele proprietarului: ";
        in >> p.nume;
        return in;
    }
};

class Medic
{
private:
    string nume; //numele de familie al doctorului
    vector<const Animal*> pacienti; //vector de pointeri catre pacientii lui, relatie de agregare
    
public:
    Medic(string nume) : nume(nume) {};
    ~Medic() {};

    void adaugaPacient(const Animal& c) {
        pacienti.push_back(&c);
    }
    
//afla cat a castigat in total medicul din toate consulturile

    int castigTotal()
    {
        int s = 0;
        for (int i = 0; i < pacienti.size(); i++)
        {
            s += pacienti[i]->getPretConsult();
        }
        return s;
    }

//afla cate animale au fiecare diagnostic existent in sistem

    void statisticaDiagnostic()
    {
            if (pacienti.empty()) { cout << "Medicul nu are pacienti!\n"; return; }
        for (int i = 0; i < pacienti.size(); i++) //trecem prin toti pacientii
        {
            bool dejaAparut = false; //presupunem ca e prima data cand vedem diagnosticul pacientului i

            for (int k = 0; k < i; k++)
            {
                if (pacienti[i]->getNumeAfectiune() == pacienti[k]->getNumeAfectiune())
                {
                    dejaAparut = true;
                    break;
                    //daca a mai avut un alt pacient din fata pacientului i acelasi diagnostic, nu mai afisam inca o data afectiunea
                }
            }
    
            if (!dejaAparut)
            {
                int cnt = 0; //numaram de cate ori apare afectiunea in vector
    
                for (int j = 0; j < pacienti.size(); j++)
                {
                    if (pacienti[i]->getNumeAfectiune() == pacienti[j]->getNumeAfectiune())
                        cnt++;
                }
    
                cout << pacienti[i]->getNumeAfectiune() << " apare de " << cnt << " ori\n";
            }
        }
    }

    friend ostream& operator<<(ostream& out, const Medic& m)
    {
        out << "Medic: Dr. " << m.nume << " (Are " << m.pacienti.size() << " pacienti inregistrati)\n";
        return out;
    }

        friend istream& operator>>(istream& in, Medic& m)
    {
        cout << "Introduceti numele medicului: ";
        in >> m.nume;
        return in;
    }

    string getNume() const
    {
        return nume;
    }

};


Animal::Animal(const Diagnostic& afectiune, string nume, int varsta) {
    this->nume = nume;
    this->afectiune = afectiune.clone();
    this->varsta = varsta;
    nrAnimale++;
}

Animal::Animal(const Animal& c)
{
    this->nume = c.nume;
    if (c.afectiune != nullptr)
    {
        afectiune = c.afectiune->clone();
    }
    else this->afectiune = nullptr;
    this->varsta = c.varsta;
    nrAnimale++;
}

//functia de citire:

void Animal::citire(istream& in)
{
    cout << " Introdu numele animalului: ";

    in >> nume;
    cout << " Introdu numele afectiunii: ";
    if (afectiune != nullptr) delete afectiune;
    string s;
    cin >> s;
    cout<<" Introdu severitatea (valoare intre 1 si 4): ";
    int x;
    cin >> x;
    while (x > 4 || x < 1)
    {
        cout << "Valoare invalida! Introduceti intre 1 si 4: ";
        cin >> x;
    }
    Grav sev = static_cast<Grav>(x);


    if (s == "Fractura")
    {
        afectiune = new Fractura(sev);
    }
    else if (s == "Dirofilarie")
    {
        afectiune = new Dirofilarie(sev);
    }
    else if (s == "Gripa")
    {
        afectiune = new Gripa(sev);
    }
    else if (s == "Obezitate")
    {
        afectiune = new Obezitate(sev);
    }
    else
    {
        afectiune = new Diagnostic(s, sev);
    }


    cout << " Introdu varsta: ";
    in >> varsta;
    if (varsta<0)
    {
        throw VarstaNegativa();
    }
}

//functia de afisare

void Animal::afisare(ostream& out) const
{
    out << "nume: " << nume << ", varsta: " << varsta << ", afectiune: " << afectiune->getNume() << endl;
}

istream& operator>>(istream& in, Animal& c)
{
    c.citire(in);
    return in;
}

ostream& operator<<(ostream& out, const Animal& c)
{
    c.afisare(out);
    return out;
}

int operator+(const Animal& c1, const Animal& c2)
{
    return int(c1.varsta+c2.varsta);
}


class Clinica
{
 //singleton
private:
    vector<Animal*> animale;
    list<Medic> medici;
    vector<Proprietar> proprietari;
    static Clinica* instance;

    //constructor privat
    Clinica() {}
public:
    Clinica(const Clinica& obj) = delete; //prevenim copiile

    ~Clinica()
    {
        for (Animal* a : animale) {
            delete a;
        }
    }
    static Clinica *getInstance()
    {
        if (instance == nullptr)
        {
            instance = new Clinica();
        }
        return instance;
    }

    Clinica& operator = (const Clinica&) = delete; //stergem operatorul =
    
    vector<Animal*>& getAnimale() { return animale; }
    list<Medic>& getMedici() { return medici; }
    vector<Proprietar>& getProprietari() { return proprietari; }

};

Clinica *Clinica::instance = 0;
int Animal::nrAnimale = 0;

void uiMeniu() {
    cout<<endl;
	cout<<"\n+++++++++++++++++++++++++++++++"<<endl;
	cout<<" Introdu 1 pentru a introduce un animal nou in sistem"<<endl;
	cout<<" Introdu 2 pentru a introduce n animale noi in sistem"<<endl;
	cout<<" Introdu 3 pentru a afisa lista tuturor animalelor/medicilor/proprietarilor "<<endl;
	cout<<" Introdu 4 pentru a face o statistica a diagnosticelor puse de un medic"<<endl;
	cout<<" Introdu 5 pentru a afisa castigul total al unui medic sau cat a platit in total un proprietar "<<endl;
    cout<<" Introdu 6 pentru a introduce un nou medic in sistem "<<endl;
    cout<<" Introdu 7 pentru a introduce un nou proprietar in sistem "<<endl;
    cout<<" Introdu 8 pentru a afisa un raport al animalelor din clinica"<<endl;

	cout<<" Pentru a iesi din program apasa 0"<<endl;
	cout<<"-----------------------------\n"<<endl;
}

template <typename T>
void afisareLista(const vector<T*>& lista)
{
    if(lista.empty())
    {
        cout<<"Nu exista inregistrari in sistem! \n";
        return;
    }

    for (const T* x : lista)
    {
        cout << *x << " " << typeid(*x).name() << endl;
    }
}

template <typename T>
void afisareLista(const vector<T>& lista) //supraincarcare ca sa mearga si pt proprietari
{
    if(lista.empty())
    {
        cout<<"Nu exista inregistrari in sistem! \n";
        return;
    }

    for (const T& x : lista)
    {
        cout << x;
    }
}

template <typename T>
void afisareLista(const list<T>& lista) //supraincarcare ca sa mearga si pt proprietari
{
    if(lista.empty())
    {
        cout<<"Nu exista inregistrari in sistem! \n";
        return;
    }

    for (const T& x : lista)
    {
        cout << x;
    }
}

void meniu ()
{
    Clinica* clinica = Clinica::getInstance();
    vector<Animal*>& animale = clinica->getAnimale();
    list<Medic>& medic = clinica->getMedici();
    vector<Proprietar>& prop = clinica->getProprietari();
	bool stop = false;
	int opt;
	while(stop == false) {
    	uiMeniu();
    	cout<<"\nIntrod opt: ";
    	cin>>opt;

    	switch (opt) {
        	case 1:
            {
            	Animal *x = nullptr;
                Diagnostic y("");
                cout<<"\n Ce specie are animalul? ";
                string specie;
                try
                {
                    cin>>specie;
                    if (specie == "Caine")
                    {
                        bool cuminte;
                        cout<< "\n Introdu 1 daca cainele este cuminte si 0 daca nu: ";
                        cin >> cuminte;
                        x = new Caine(y, "", 0, cuminte); //upcast
                    }
                    else if (specie == "Pisica") x = new Pisica(y, "", 0);
                    else if (specie == "Hamster") x = new Hamster(y, "", 0);
                    else throw specieNecunoscuta();
                    cin >> *x;
                    animale.push_back(x);
                    cout << "\n Animal adaugat cu succes!";
                    
                    cout << "\n Introduceti numele proprietarului: ";
                    string numeprop;
                    cin >> numeprop;

                    bool gasitprop = false;
                    for (Proprietar& p : prop)
                    {
                        if (p.getNume() == numeprop)
                        {
                            gasitprop = true;
                            p.adaugaAnimal(*x);
                            cout << " Animalul a fost atribuit proprietarului! "<< endl;
                            break;
                        }
                    }
                    if (!gasitprop) cout << "  Proprietarul " << numeprop << " nu este inregistrat";

                    cout << "\n Introduceti numele medicului: ";
                    string numemed;
                    cin >> numemed;

                    bool gasitmed = false;
                    for (Medic& m : medic)
                    {
                        if (m.getNume() == numemed)
                        {
                            m.adaugaPacient(*x);
                            cout << " Animalul a fost atribuit medicului! "<< endl;
                            gasitmed = true;
                            break;
                        }
                    }
                    if (!gasitmed) cout << " Medicul " << numemed << " nu este inregistrat";
                }
                catch (const specieNecunoscuta& e)
                {
                    cout << "EROARE! " << e.what();
                    delete x; //abandonam x daca are specia invalida
                }

                catch (const VarstaNegativa& e)
                {
                    cout << "EROARE! " << e.what();
                    delete x; //abandonam x daca are varsta negativa
                }

                
            	break;
            }
        	case 2:
        	{
        	    int n, a, i;
                i = animale.size();

  	            cout<<"\nIntroduceti n: ";
  	            cin>>n;
  	            a = n;
  	            while(n)
  	            {
  	                cout<<"animal "<<a-n+1<<endl;
                    Animal *x = nullptr;
                    Diagnostic y("");
                    cout<<"\n Ce specie are animalul? ";
                    string specie;
                    try
                    {
                        cin>>specie;
                        if (specie == "Caine")
                        {
                            bool cuminte;
                            cout<< "\n Introdu 1 daca cainele este cuminte si 0 daca nu: ";
                            cin >> cuminte;
                            x = new Caine(y, "", 0, cuminte);
                        }
                        else if (specie == "Pisica") x = new Pisica(y, "", 0);
                        else if (specie == "Hamster") x = new Hamster(y, "", 0);
                        else throw specieNecunoscuta();
                        cin >> *x;
                        animale.push_back(x);
                        cout << "\nAnimal adaugat cu succes!";
                        cout << "\nIntroduceti numele proprietarului: ";
                        string numeprop;
                        cin >> numeprop;

                        bool gasitprop = false;
                        for (Proprietar& p : prop)
                        {
                            if (p.getNume() == numeprop)
                            {
                                gasitprop = true;
                                p.adaugaAnimal(*x);
                                cout << "Animalul a fost atribuit proprietarului! "<< endl;
                                break;
                            }
                        }
                        if (!gasitprop) cout << "Proprietarul " << numeprop << " nu este inregistrat";

                        cout << "\nIntroduceti numele medicului: ";
                        string numemed;
                        cin >> numemed;

                        bool gasitmed = false;
                        for (Medic& m : medic)
                        {
                            if (m.getNume() == numemed)
                            {
                                m.adaugaPacient(*x);
                                cout << "Animalul a fost atribuit medicului! "<< endl;
                                gasitmed = true;
                                break;
                            }
                        }
                        if (!gasitmed) cout << "Medicul " << numemed << " nu este inregistrat";
                    
                    }
                    catch (const specieNecunoscuta& e)
                    {
                        cout << "EROARE! " << e.what();
                        delete x; //abandonam x daca are specia invalida
                    }
                    
                    catch (const VarstaNegativa& e)
                    {
                        cout<< "EROARE! " << e.what();
                        delete x; //abandonam x daca are varsta negativa
                    }

                	n--;
  	            }
  	            cout<<endl;
                cout<<"\nAnimalele adaugate: \n";
                while(i < animale.size())
                {
                    cout << *animale[i];
                    i++;
                }
            	break;
        	}
        	case 3:
           	{
                cout << "Introdu ce lista vrei sa fie afisata (Animale/Proprietari/Medici)\n";
                string lista;
                cin >> lista;
                if (lista == "Animale")
                {
                    cout << "LISTA ANIMALE:"<< endl;
                    afisareLista(animale);
                    break;
                }
                else if (lista == "Proprietari")
                {
                    cout << "LISTA PROPRIETARI:"<< endl;
                    afisareLista(prop);
                    break;
                }
                else if (lista == "Medici")
                {
                    cout << "LISTA MEDICI:"<< endl;
                    afisareLista(medic);
                    break;
                }
                else
                {
                    cout << "Optiune invalida!" << endl;
                }
                break;
            }

        	case 4:
        	{
                cout<< "Introdu numele medicului: " << endl;
                string numemed;
                cin >> numemed;
                    bool gasitmed = false;
                    for (Medic& m : medic)
                    {
                        if (m.getNume() == numemed)
                        {
                        cout << "\nStatistica Afectiuni: \n";
                            m.statisticaDiagnostic();
                            gasitmed = true;
                            break;
                        }
                    }
                    if (!gasitmed) cout << "Medicul " << numemed << " nu este inregistrat";
                
                break;
            }

        	case 5:
            {
                string s;
                cout<<"Ce vreti sa afisati? (Proprietar/Medic)\n";
                cin>>s;
                if (s=="Proprietar")
                {
                    cout << "\nIntroduceti numele proprietarului: ";
                    string numeprop;
                    cin >> numeprop;
                    bool gasitprop = false;
                    for (Proprietar& p : prop)
                    {
                        if (p.getNume() == numeprop)
                        {
                            p.plataTotala();
                            gasitprop = true;
                            break;
                        }
                    }
                    if (!gasitprop) cout << "Proprietarul nu este inregistrat!";
                }
                else if (s=="Medic")
                {
                    cout << "\nIntroduceti numele medicului: ";
                    string numemed;
                    cin >> numemed;
                    bool gasitmed = false;
                    for (Medic& m : medic)
                    {
                        if (m.getNume() == numemed)
                        {
                            cout << "Total incasat: " << m.castigTotal() << " RON\n";
                            gasitmed = true;
                            break;
                        }
                    }
                    if (!gasitmed) cout<<"Medicul nu este inregistrat!";
                }
                else cout<<"Introdu 'Medic' sau 'Proprietar'!"<<endl;
                break;
            }
            
            case 6:
            {
                Medic m("");
                cin >> m;
                clinica->getMedici().push_back(m);
                cout << "Medic adaugat cu succes!\n";
                break;
            }

            case 7:
            {
                Proprietar p(vector<Animal*>(), "");
                cin >> p;
                clinica->getProprietari().push_back(p);
                cout << "Proprietar adaugat cu succes!";
                break;
            }

            case 8:
            {
                Animal::raportClinica(animale);
                break;
            }
    
        	case 0:
           	 
            	stop = true;
            	break;
        	default:
            	cout << "Optiune nevalida! Incearca din nou.\n";
    	}

    	system("pause");
    	system("cls");

	}
}


int main()
{
    meniu();
    return 0;
}