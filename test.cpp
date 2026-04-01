#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// --- SABITLER ---
const int MAX_DERSLIK_SAYISI = 100; 
const int SONSUZ = 999999; 

// --- YARDIMCI FONKSIYONLAR ---

// .length() yerine manuel uzunluk hesaplayici
int metinUzunlugu(string metin) {
    int sayac = 0;
    while (metin[sayac] != '\0') {
        sayac++;
    }
    return sayac;
}

// Metni sayiya cevirir (Manuel String to Integer)
int metniSayiyaCevir(string metin) {
    if(metinUzunlugu(metin) == 0) return 0;
    int sonuc = 0;
    int i = 0;
    int isaret = 1;
    
    int uzunluk = metinUzunlugu(metin);

    // Bosluklari atla
    while(i < uzunluk && (metin[i] <= ' ')) i++;

    if (i < uzunluk && metin[i] == '-') {
        isaret = -1;
        i++;
    }

    for (; i < uzunluk; ++i) {
        if (metin[i] >= '0' && metin[i] <= '9') {
            sonuc = sonuc * 10 + (metin[i] - '0');
        } else {
            break; 
        }
    }
    return sonuc * isaret;
}

// Sayiyi metne cevirir (Manuel Integer to String - Listeleme icin gerekli)
string sayiMetne(int sayi) {
    if (sayi == 0) return "0";
    string s = "";
    int temp = sayi;
    if(temp < 0) temp = -temp;
    
    while(temp > 0) {
        char c = '0' + (temp % 10);
        string gecici = " "; // char'i stringe eklemek icin hile
        gecici[0] = c;
        s = gecici + s;
        temp /= 10;
    }
    if(sayi < 0) s = "-" + s;
    return s;
}

// YENI: Tablo sutunlarini duzgun hizalamak icin bosluk ekler
void yazdirHizali(string metin, int genislik) {
    cout << metin;
    int len = metinUzunlugu(metin);
    // Eger metin cok uzunsa ve genisligi asiyorsa diye koruma
    if (len >= genislik) {
        cout << " "; // En az 1 bosluk birak
    } else {
        // Kalan kisim kadar bosluk bas
        for(int k=0; k < (genislik - len); k++) cout << " ";
    }
}

// Bir satiri kelimelere bolmek icin yardimci
void satiriBol(string satir, string kelimeler[], int &kelimeSayisi) {
    kelimeSayisi = 0;
    string geciciKelime = "";
    int uzunluk = metinUzunlugu(satir); 

    for(int i=0; i<uzunluk; i++) {
        if(satir[i] == ' ' || satir[i] == '\t' || satir[i] == '\r' || satir[i] == '\n') {
            if(metinUzunlugu(geciciKelime) > 0) {
                kelimeler[kelimeSayisi++] = geciciKelime;
                geciciKelime = "";
            }
        } else {
            geciciKelime += satir[i];
        }
    }
    if(metinUzunlugu(geciciKelime) > 0) kelimeler[kelimeSayisi++] = geciciKelime;
}

// --- DINAMIK DIZI YAPISI ---
template <typename T>
class DinamikDizi {
private:
    T* veri;
    int boyut;
    int kapasite;

public:
    DinamikDizi() {
        kapasite = 20;
        boyut = 0;
        veri = new T[kapasite];
    }

    ~DinamikDizi() {
        if(veri) delete[] veri;
    }

    void ekle(T eleman) {
        if (boyut >= kapasite) {
            kapasite *= 2;
            T* yeniVeri = new T[kapasite];
            for (int i = 0; i < boyut; i++) {
                yeniVeri[i] = veri[i];
            }
            delete[] veri;
            veri = yeniVeri;
        }
        veri[boyut++] = eleman;
    }

    T& getir(int index) {
        return veri[index];
    }

    int getBoyut() const {
        return boyut;
    }

    void temizle() {
        boyut = 0;
    }
};

// --- SINIF TANIMLARI ---

class Ders {
public:
    int id;
    string ad;
    string tip; 
    int akts;
    int onSartDersID; 
    int derslikID;
    int onemPuani;

    Ders() : id(0), akts(0), onSartDersID(0), derslikID(0), onemPuani(0) {}
};

class Ogrenci {
public:
    int id;
    string ad;     
    int maxAKTS;

    Ogrenci() : id(0), maxAKTS(0) {}
};

// --- YÖNETİM SİSTEMİ ---

class DersYonetimSistemi {
private:
    DinamikDizi<Ders> tumDersler;
    DinamikDizi<Ogrenci> tumOgrenciler;
    
    DinamikDizi<Ders> secilenDersler;      
    DinamikDizi<Ders> siraliDersler;       
    
    int komsuMatrisi[MAX_DERSLIK_SAYISI][MAX_DERSLIK_SAYISI];
    int derslikIDListesi[MAX_DERSLIK_SAYISI];
    int toplamDerslikSayisi;

    int derslikIndeksiniBul(int id) {
        for(int i=0; i<toplamDerslikSayisi; i++) {
            if(derslikIDListesi[i] == id) return i;
        }
        return -1;
    }

public:
    DersYonetimSistemi() {
        toplamDerslikSayisi = 0;
        for(int i=0; i<MAX_DERSLIK_SAYISI; i++)
            for(int j=0; j<MAX_DERSLIK_SAYISI; j++)
                komsuMatrisi[i][j] = (i==j) ? 0 : SONSUZ;
    }

    void dosyalariOku() {
        string satirKelimeleri[50]; 
        int kelimeSayisi = 0;
        string okunanSatir; 

        // --- 1. OGRENCILER DOSYASI ---
        ifstream dosyaOgrenci("ogrenciler.txt"); 
        if (!dosyaOgrenci.is_open()) {
            cerr << "HATA: ogrenciler.txt acilamadi!" << endl;
        } else {
            getline(dosyaOgrenci, okunanSatir); 
            while(getline(dosyaOgrenci, okunanSatir)) {
                satiriBol(okunanSatir, satirKelimeleri, kelimeSayisi);
                if(kelimeSayisi >= 3) { 
                    Ogrenci ogr;
                    ogr.id = metniSayiyaCevir(satirKelimeleri[0]);
                    ogr.maxAKTS = metniSayiyaCevir(satirKelimeleri[kelimeSayisi-1]); 
                    
                    ogr.ad = "";
                    for(int i=1; i < kelimeSayisi-1; i++) {
                        ogr.ad += satirKelimeleri[i] + " ";
                    }
                    if(metinUzunlugu(ogr.ad) > 0) {
                         string geciciAd = "";
                         int len = metinUzunlugu(ogr.ad);
                         for(int k=0; k<len-1; k++) geciciAd += ogr.ad[k];
                         ogr.ad = geciciAd;
                    }
                    tumOgrenciler.ekle(ogr);
                }
            }
            dosyaOgrenci.close();
        }

        // --- 2. DERSLER DOSYASI ---
        ifstream dosyaDers("dersler.txt");
        if (!dosyaDers.is_open()) {
            cerr << "HATA: dersler.txt acilamadi!" << endl;
        } else {
            getline(dosyaDers, okunanSatir); 
            while(getline(dosyaDers, okunanSatir)) {
                satiriBol(okunanSatir, satirKelimeleri, kelimeSayisi);
                if(kelimeSayisi >= 5) {
                    Ders d;
                    d.id = metniSayiyaCevir(satirKelimeleri[0]);
                    d.derslikID = metniSayiyaCevir(satirKelimeleri[kelimeSayisi-1]);
                    
                    int tipIndeksi = -1;
                    for(int i=kelimeSayisi-1; i>=0; i--) {
                        if(satirKelimeleri[i] == "Z" || satirKelimeleri[i] == "S") {
                            tipIndeksi = i;
                            break;
                        }
                    }
                    
                    if(tipIndeksi != -1) {
                        d.tip = satirKelimeleri[tipIndeksi];
                        d.akts = metniSayiyaCevir(satirKelimeleri[tipIndeksi+1]);
                        
                        if((kelimeSayisi - 1) == (tipIndeksi + 2)) {
                            d.onSartDersID = 0;
                        } else {
                            d.onSartDersID = metniSayiyaCevir(satirKelimeleri[tipIndeksi+2]);
                        }
                        
                        d.ad = "";
                        for(int i=1; i < tipIndeksi; i++) {
                            d.ad += satirKelimeleri[i] + " ";
                        }
                        if(metinUzunlugu(d.ad) > 0) {
                             string geciciAd = "";
                             int len = metinUzunlugu(d.ad);
                             for(int k=0; k<len-1; k++) geciciAd += d.ad[k];
                             d.ad = geciciAd;
                        }
                        tumDersler.ekle(d);
                    }
                }
            }
            dosyaDers.close();
        }

        // --- 3. DERSLIKLER DOSYASI ---
        ifstream dosyaDerslik("derslikler.txt");
        if(!dosyaDerslik.is_open()) {
             cerr << "HATA: derslikler.txt acilamadi!" << endl;
        } else {
            getline(dosyaDerslik, okunanSatir); 
            int matrisSatirSayaci = 0; 
            while(getline(dosyaDerslik, okunanSatir) && matrisSatirSayaci < MAX_DERSLIK_SAYISI) {
                satiriBol(okunanSatir, satirKelimeleri, kelimeSayisi);
                if(kelimeSayisi > 0) {
                    derslikIDListesi[matrisSatirSayaci] = metniSayiyaCevir(satirKelimeleri[0]); 
                    for(int j=1; j<kelimeSayisi; j++) {
                        if(j-1 < MAX_DERSLIK_SAYISI) {
                             if(satirKelimeleri[j] == "*") komsuMatrisi[matrisSatirSayaci][j-1] = SONSUZ;
                             else komsuMatrisi[matrisSatirSayaci][j-1] = metniSayiyaCevir(satirKelimeleri[j]);
                        }
                    }
                    matrisSatirSayaci++;
                }
            }
            toplamDerslikSayisi = matrisSatirSayaci;
            dosyaDerslik.close();
        }
    }

    void onemPuaniHesapla() {
        for(int i=0; i<tumDersler.getBoyut(); i++) {
            Ders& d = tumDersler.getir(i);
            d.onemPuani = d.akts;
            if(d.tip == "Z") d.onemPuani += 20;
            else if(d.tip == "S") d.onemPuani += 10;
        }
        for(int i=0; i<tumDersler.getBoyut(); i++) {
            int mevcutID = tumDersler.getir(i).id;
            for(int j=0; j<tumDersler.getBoyut(); j++) {
                if(tumDersler.getir(j).onSartDersID == mevcutID) {
                    tumDersler.getir(i).onemPuani += 5;
                }
            }
        }
    }

    void dersSecimiKnapsack(Ogrenci& ogr) {
        secilenDersler.temizle();
        int n = tumDersler.getBoyut();
        int kapasite = ogr.maxAKTS;
        
        int** dpTablosu = new int*[n + 1];
        for(int i = 0; i <= n; ++i)
            dpTablosu[i] = new int[kapasite + 1];

        for (int i = 0; i <= n; i++) {
            for (int w = 0; w <= kapasite; w++) {
                if (i == 0 || w == 0)
                    dpTablosu[i][w] = 0;
                else if (tumDersler.getir(i - 1).akts <= w) {
                    int puan = tumDersler.getir(i - 1).onemPuani;
                    int kalanKapasite = w - tumDersler.getir(i - 1).akts;
                    if (puan + dpTablosu[i - 1][kalanKapasite] > dpTablosu[i - 1][w])
                        dpTablosu[i][w] = puan + dpTablosu[i - 1][kalanKapasite];
                    else
                        dpTablosu[i][w] = dpTablosu[i - 1][w];
                } else {
                    dpTablosu[i][w] = dpTablosu[i - 1][w];
                }
            }
        }

        int kalanPuan = dpTablosu[n][kapasite];
        int w = kapasite;
        for (int i = n; i > 0 && kalanPuan > 0; i--) {
            if (kalanPuan == dpTablosu[i - 1][w])
                continue;
            else {
                secilenDersler.ekle(tumDersler.getir(i - 1));
                kalanPuan -= tumDersler.getir(i - 1).onemPuani;
                w -= tumDersler.getir(i - 1).akts;
            }
        }

        for(int i = 0; i <= n; ++i) delete[] dpTablosu[i];
        delete[] dpTablosu;
    }

    int enKisaMesafe(int baslangicNodeID, int bitisNodeID) {
        if(baslangicNodeID == bitisNodeID) return 0;
        int baslangicIndeks = derslikIndeksiniBul(baslangicNodeID);
        int bitisIndeks = derslikIndeksiniBul(bitisNodeID);
        
        if(baslangicIndeks == -1 || bitisIndeks == -1) return SONSUZ;

        int enKisaMesafeler[MAX_DERSLIK_SAYISI];
        bool ziyaretEdildi[MAX_DERSLIK_SAYISI]; 

        for(int i=0; i<toplamDerslikSayisi; i++) {
            enKisaMesafeler[i] = SONSUZ;
            ziyaretEdildi[i] = false;
        }
        enKisaMesafeler[baslangicIndeks] = 0;

        for(int count = 0; count < toplamDerslikSayisi - 1; count++) {
            int minMesafe = SONSUZ, u = -1;
            for(int v=0; v < toplamDerslikSayisi; v++) {
                if(!ziyaretEdildi[v] && enKisaMesafeler[v] <= minMesafe) {
                    minMesafe = enKisaMesafeler[v];
                    u = v;
                }
            }
            if(u == -1) break;
            ziyaretEdildi[u] = true;

            for(int v=0; v < toplamDerslikSayisi; v++) {
                if(!ziyaretEdildi[v] && komsuMatrisi[u][v] != SONSUZ && enKisaMesafeler[u] != SONSUZ 
                   && enKisaMesafeler[u] + komsuMatrisi[u][v] < enKisaMesafeler[v]) {
                    enKisaMesafeler[v] = enKisaMesafeler[u] + komsuMatrisi[u][v];
                }
            }
        }
        return enKisaMesafeler[bitisIndeks];
    }

    void rotaOlustur() {
        siraliDersler.temizle();
        int suankiKonumID = 100; 
        
        DinamikDizi<Ders> geciciListe;
        for(int i=0; i<secilenDersler.getBoyut(); i++) 
            geciciListe.ekle(secilenDersler.getir(i));
        
        while(geciciListe.getBoyut() > 0) {
            int enYakinIndeks = -1;
            int enKisaMes = SONSUZ;

            for(int i=0; i<geciciListe.getBoyut(); i++) {
                int mesafe = enKisaMesafe(suankiKonumID, geciciListe.getir(i).derslikID);
                if(mesafe < enKisaMes) {
                    enKisaMes = mesafe;
                    enYakinIndeks = i;
                }
            }

            if(enYakinIndeks != -1) {
                Ders secilen = geciciListe.getir(enYakinIndeks);
                siraliDersler.ekle(secilen);
                suankiKonumID = secilen.derslikID;

                DinamikDizi<Ders> yeniGecici;
                for(int k=0; k<geciciListe.getBoyut(); k++) {
                    if(k != enYakinIndeks) yeniGecici.ekle(geciciListe.getir(k));
                }
                geciciListe.temizle();
                for(int k=0; k<yeniGecici.getBoyut(); k++) geciciListe.ekle(yeniGecici.getir(k));
            } else {
                siraliDersler.ekle(geciciListe.getir(0));
                
                 DinamikDizi<Ders> yeniGecici;
                 for(int k=1; k<geciciListe.getBoyut(); k++) yeniGecici.ekle(geciciListe.getir(k));
                 geciciListe.temizle();
                 for(int k=0; k<yeniGecici.getBoyut(); k++) geciciListe.ekle(yeniGecici.getir(k));
            }
        }
    }

    // --- YAZDIRMA FONKSIYONLARI ---
    void yazdirDersKaydi(ostream& out, Ogrenci& ogr) {
        int toplamAKTS = 0;
        int toplamPuan = 0;

        out << "OGRENCI: " << ogr.ad << endl;
        out << "AKTS LIMITI: " << ogr.maxAKTS << endl;
        out << "SECILEN DERSLER:" << endl;
        
        for(int i=0; i<secilenDersler.getBoyut(); i++) {
            Ders& d = secilenDersler.getir(i);
            out << "  " << (i+1) << ". " << d.ad 
                << " (AKTS: " << d.akts 
                << ", Puan: " << d.onemPuani 
                << ", Derslik: " << d.derslikID << ")" << endl;
            toplamAKTS += d.akts;
            toplamPuan += d.onemPuani;
        }
        
        out << "Toplam AKTS: " << toplamAKTS << endl;
        out << "Toplam Puan: " << toplamPuan << endl;
        out << "Program Uygun: EVET" << endl;
        out << "------------------------------------" << endl;
        out << endl; 
    }

    void yazdirRapor(ostream& out, Ogrenci& ogr) {
        out << "OGRENCI: " << ogr.ad << endl;
        out << "AKTS LIMITI: " << ogr.maxAKTS << endl;
        
        out << "IZLENEN ROTA: [100]"; 
        for(int i=0; i<siraliDersler.getBoyut(); i++) {
            out << " -> [" << siraliDersler.getir(i).derslikID << "]";
        }
        out << endl;
        out << "------------------------------------" << endl;
        
        int toplamYurume = 0;
        int suankiKonumID = 100;
        int suankiSaatDk = 465; 
        
        for(int i=0; i<siraliDersler.getBoyut(); i++) {
            Ders& d = siraliDersler.getir(i);
            int mesafe = enKisaMesafe(suankiKonumID, d.derslikID);
            if(mesafe != SONSUZ) {
                toplamYurume += mesafe;
                suankiKonumID = d.derslikID;
            }
        }

        out << "TOPLAM YURUME MESAFESI: " << toplamYurume << " metre" << endl;
        out << "------------------------------------" << endl;
        out << "GIRILEN DERSLER (" << siraliDersler.getBoyut() << "):" << endl;

        suankiKonumID = 100;
        suankiSaatDk = 465;
        
        for(int i=0; i<siraliDersler.getBoyut(); i++) {
            Ders& d = siraliDersler.getir(i);
            int mesafe = enKisaMesafe(suankiKonumID, d.derslikID);
            if(mesafe == SONSUZ) mesafe = 0; 

            suankiSaatDk += (int)(mesafe / 80.0);
            
            int hedefBaslangic = ((suankiSaatDk + 59) / 60) * 60;
            if(hedefBaslangic < 480) hedefBaslangic = 480; 
            if(suankiSaatDk > hedefBaslangic) hedefBaslangic += 60;

            out << "  " << (i+1) << ". " << d.ad 
                << " (Derslik: " << d.derslikID 
                << ", Saat: " << (hedefBaslangic/60) << ":00)" << endl;

            suankiKonumID = d.derslikID;
            suankiSaatDk = hedefBaslangic + 50;
        }

        out << "DURUM: TAMAMLANDI" << endl;
        out << "====================================" << endl;
        out << endl;
    }

    // --- YENI: LISTELEME VE MENU FONKSIYONLARI (DUZELTILDI) ---

    void dersleriListele() {
        cout << "\n--- TUM DERSLER LISTESI ---\n";
        // Basliklar icin elle ayarlanmis genislikler
        yazdirHizali("ID", 5);
        yazdirHizali("Ders Adi", 40); // Ders adina genis yer ayirdik
        yazdirHizali("Tip", 5);
        yazdirHizali("AKTS", 7);
        yazdirHizali("On Sart ID", 12);
        cout << endl;
        cout << "----------------------------------------------------------------------\n";
        
        for(int i=0; i<tumDersler.getBoyut(); i++) {
            Ders& d = tumDersler.getir(i);
            yazdirHizali(sayiMetne(d.id), 5);
            yazdirHizali(d.ad, 40);
            yazdirHizali(d.tip, 5);
            yazdirHizali(sayiMetne(d.akts), 7);
            
            if(d.onSartDersID == 0) yazdirHizali("Yok", 12);
            else yazdirHizali(sayiMetne(d.onSartDersID), 12);
            
            cout << endl;
        }
        cout << "----------------------------------------------------------------------\n\n";
    }

    void ogrencileriListele() {
        cout << "\n--- TUM OGRENCILER LISTESI ---\n";
        yazdirHizali("ID", 5);
        yazdirHizali("Ogrenci Adi", 30);
        yazdirHizali("Max AKTS", 10);
        cout << endl;
        cout << "---------------------------------------------\n";
        
        for(int i=0; i<tumOgrenciler.getBoyut(); i++) {
            Ogrenci& ogr = tumOgrenciler.getir(i);
            yazdirHizali(sayiMetne(ogr.id), 5);
            yazdirHizali(ogr.ad, 30);
            yazdirHizali(sayiMetne(ogr.maxAKTS), 10);
            cout << endl;
        }
        cout << "---------------------------------------------\n\n";
    }

    void ogrenciSecVeRaporla() {
        int arananID;
        cout << "\n> Raporunu gormek istediginiz Ogrenci ID'sini giriniz: ";
        cin >> arananID;

        // Ogrenciyi Bul
        int bulunanIndeks = -1;
        for(int i=0; i<tumOgrenciler.getBoyut(); i++) {
            if(tumOgrenciler.getir(i).id == arananID) {
                bulunanIndeks = i;
                break;
            }
        }

        if(bulunanIndeks != -1) {
            Ogrenci& ogr = tumOgrenciler.getir(bulunanIndeks);
            cout << "\n>> " << ogr.ad << " icin hesaplama yapiliyor...\n";
            
            dersSecimiKnapsack(ogr);
            rotaOlustur();

            cout << "\n******* DERS KAYDI RAPORU (EKRAN) *******\n";
            yazdirDersKaydi(cout, ogr); 
            
            cout << "\n******* ROTA SIMULASYON RAPORU (EKRAN) *******\n";
            yazdirRapor(cout, ogr); 
            
            cout << "*****************************************\n";
        } else {
            cout << "HATA: Girdiginiz ID'ye sahip ogrenci bulunamadi!\n";
        }
    }

    void anaMenu() {
        int secim = -1;
        while(secim != 0) {
            cout << "\n=== DERS KAYIT SISTEMI ANA MENU ===\n";
            cout << "1. Dersleri Listele\n";
            cout << "2. Ogrencileri Listele\n";
            cout << "3. Ogrenci Sec ve Rapor Goruntule\n";
            cout << "4. Dosyalari Olustur (Tum Ogrenciler Icin)\n";
            cout << "0. Cikis\n";
            cout << "Seciminiz: ";
            cin >> secim;

            if(secim == 1) {
                dersleriListele();
            } else if(secim == 2) {
                ogrencileriListele();
            } else if(secim == 3) {
                ogrenciSecVeRaporla();
            } else if(secim == 4) {
                // Dosyalari Sifirla
                ofstream fKayit("dersKaydi.txt"); fKayit << "DERS KAYITLARI\n\n"; fKayit.close();
                ofstream fRapor("rapor.txt"); fRapor << "RAPORLAR\n\n"; fRapor.close();
                
                cout << ">> Tum ogrenciler icin dosyalar olusturuluyor...\n";
                for(int i=0; i<tumOgrenciler.getBoyut(); i++) {
                    Ogrenci& ogr = tumOgrenciler.getir(i);
                    dersSecimiKnapsack(ogr);
                    rotaOlustur();
                    
                    ofstream outKayit("dersKaydi.txt", ios::app);
                    yazdirDersKaydi(outKayit, ogr);
                    outKayit.close();

                    ofstream outRapor("rapor.txt", ios::app);
                    yazdirRapor(outRapor, ogr);
                    outRapor.close();
                }
                cout << ">> Islem Tamamlandi. Dosyalar kaydedildi.\n";
            } else if(secim == 0) {
                cout << "Programdan cikiliyor...\n";
            } else {
                cout << "Gecersiz secim!\n";
            }
        }
    }

    void Baslat() {
        dosyalariOku();
        if(tumOgrenciler.getBoyut() == 0) {
            cout << "Veri okunamadi veya dosya bos." << endl;
        }
        
        onemPuaniHesapla();
        anaMenu(); 
    }
};

int main() {
    DersYonetimSistemi sistem;
    sistem.Baslat();
    return 0;
}