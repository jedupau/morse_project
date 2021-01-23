#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <vector>
#define duree_ti 0.5
#define duree_ta 1.5
#define inter_signe 0.5
#define inter_lettre 1
#define inter_mot 3.5
using namespace std;
namespace little_endian_io
{
    template <typename Word>
    std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
    {
        for (; size; --size, value >>= 8)
        outs.put( static_cast <char> (value & 0xFF) );
        return outs;
    }
}
using namespace little_endian_io;

map<string, string> gen_dict1()
{
    map<string, string> dict;
    dict["a"] = "01";
    dict["b"] = "1000";
    dict["c"] = "1010";
    dict["d"] = "100";
    dict["e"] = "0";
    dict["f"] = "0010";
    dict["g"] = "110";
    dict["h"] = "0000";
    dict["i"] = "00";
    dict["j"] = "0111";
    dict["k"] = "101";
    dict["l"] = "0100";
    dict["m"] = "11";
    dict["n"] = "10";
    dict["o"] = "111";
    dict["p"] = "0110";
    dict["q"] = "1101";
    dict["r"] = "010";
    dict["s"] = "000";
    dict["t"] = "1";
    dict["u"] = "001";
    dict["v"] = "0001";
    dict["w"] = "011";
    dict["x"] = "1001";
    dict["y"] = "1011";
    dict["z"] = "1100";
    return dict;
}
map<string, string> gen_dict2()
{
    map<string, string> dict;
    dict["01"] = "a";
    dict["1000"] = "b";
    dict["1010"] = "c";
    dict["100"] = "d";
    dict["0"] = "e";
    dict["0010"] = "f";
    dict["110"] = "g";
    dict["0000"] = "h";
    dict["00"] = "i";
    dict["0111"] = "j";
    dict["101"] = "k";
    dict["0100"] = "l";
    dict["11"] = "m";
    dict["10"] = "n";
    dict["111"] = "o";
    dict["0110"] = "p";
    dict["1101"] = "q";
    dict["010"] = "r";
    dict["000"] = "s";
    dict["1"] = "t";
    dict["001"] = "u";
    dict["0001"] = "v";
    dict["011"] = "w";
    dict["1001"] = "x";
    dict["1011"] = "y";
    dict["1100"] = "z";
    return dict;
}
void initialiser_fichier(ofstream& f)
{
    f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
    write_word( f,     16, 4 );  // no extension data
    write_word( f,      1, 2 );  // PCM - integer samples
    write_word( f,      2, 2 );  // two channels (stereo file)
    write_word( f,  44100, 4 );  // samples per second (Hz)
    write_word( f, 176400, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
    write_word( f,      4, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
    write_word( f,     16, 2 );  // number of bits per sample (use a multiple of 8)
    // Write the data chunk header
    size_t data_chunk_pos = f.tellp();
    f << "data----";  // (chunk size to be filled in later)
    
}
void ecrire_fichier(ofstream& f, char const c)
{
    // Write the audio samples
    // (We'll generate a single C4 note with a sine wave, fading from left to right)
    constexpr double two_pi = 6.283185307179586476925286766559;
    constexpr double max_amplitude = 32760;  // "volume"

    double hz        = 44100;    // samples per second
    double frequency = 440;  // middle C
    double seconds;      // time
    
    int N;  // total number of samples
    double amplitude;
    double value;
    if (c=='0')
    {
        seconds = duree_ti;
    }
    else if (c=='1')
    {
        seconds = duree_ta;
    }
    else if (c=='/')
    {
        seconds = inter_lettre;
    }
    N = seconds*hz;
    if (c!= '/')
    {
        for (int n = 0; n < N; n++)
        {
            amplitude = (double)n / N * max_amplitude;
            value     = sin( (two_pi * n * frequency) / hz );
            
            write_word( f, int(amplitude  * value), 2 );
            write_word( f, int((max_amplitude - amplitude) * value), 2 );            
        }
    }
    else 
    {
        for (int n = 0; n < N; n++)
        {
            write_word( f, 0, 2 );
            write_word( f, 0, 2 );
        }
    }
    seconds = inter_signe;
    N = seconds * hz;
    for (int n = 0; n < N; n++)
    {
        write_word( f, 0, 2 );
        write_word( f, 0, 2 );

    }
}


void terminer_fichier(ofstream& f)
{
    size_t data_chunk_pos = f.tellp();
    // (We'll need the final file size to fix the chunk sizes above)
    size_t file_length = f.tellp();

    // Fix the data chunk header to contain the data size
    f.seekp( data_chunk_pos + 4 );
    write_word( f, file_length - data_chunk_pos + 8 );

    // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
    f.seekp( 0 + 4 );
    write_word( f, file_length - 8, 4 );
}

string lire_fichier(ifstream& f)
{
    map<string, string> dict = gen_dict2();
    f.seekg(0, ios::end);
    long int taille_fichier = f.tellg();
    f.seekg(0, ios::beg);           
    char c[44];
    f.read((char*)c, 44);   
    int t;
    vector<int> zero;
    int n;
    string result;
    string mot;
    for(int i =0; i<taille_fichier-44;i+=2)
    {
        f.read((char*)&t, 2);
        if(t==0)
        {
            n ++;    
        }
        else
        {
            if(n > 2*(inter_mot)*44000)
            {
                
                if(zero.size() > 0)
                {
                    if((i - zero[zero.size() - 1])/2 > 2*duree_ta*44000 + 2*inter_mot*44000)
                    {
                        result += "1";
                    }
                    else
                    {
                        result += "0";
                    }
                }
                else
                {
                    if(i/2 > 2*duree_ta*44000 + 2*inter_mot*44000)
                    {
                        result += "1";
                    }
                    else
                    {
                        result += "0";
                    }
                }
                mot += dict[result];
                mot += " ";
                result = "";
                
                zero.push_back(i);
            }
            else if(n>2*(2*inter_signe +inter_lettre)*44000)
            {
                
                if(zero.size() > 0)
                {
                    if((i - zero[zero.size() - 1])/2 > 2*duree_ta*44000 + 2*(2*inter_signe + inter_lettre)*44000)
                    {
                        result += "1";
                    }
                    else
                    {
                        result += "0";
                    }
                }
                else
                {
                    if(i/2 > 2*duree_ta*44000 + 2*(2*inter_signe + inter_lettre)*44000)
                    {
                        result += "1";
                    }
                    else
                    {
                        result += "0";
                    }
                }
                mot += dict[result];
                result = "" ;
                zero.push_back(i);
            }
            else if (n>2*inter_signe*44000)
            {
                if(zero.size() > 0)
                {
                    if((i - zero[zero.size() - 1])/2 > 2*duree_ta*44000 + 2*inter_signe*44000)
                    {
                        result += "1";
                    }
                    else
                    {
                        result += "0";
                    }
                }
                else
                {
                    if(i/2 > 2*duree_ta*44000 + 2*inter_signe*44000)
                    {
                        result += "1";
                    }
                    else
                    {
                        result += "0";
                    }
                }
                zero.push_back(i);
            }
            n = 0;   
        }
    }
    return mot;
}
int main () {
    cout << "Que voulez vous faire ?  ";
    string action;
    getline(cin,action);
    cout << endl;
    if (action == "encoder")
    {
        map<string, string> dict = gen_dict1();
        cout << "Quel texte voulez vous encoder ?  ";
        string mot;
        getline(cin,mot);
        cout << endl;
        string result;
        string lettre;
        for (char const c: mot)
        {
            lettre = c;
            result += dict[lettre];
            result += "/";
        }
        cout << "Quel va être le nom du fichier ?  ";

        getline(cin, mot);
        ofstream f(mot + ".wav", ios::binary);
        initialiser_fichier(f);
        for(char const c: result)
        {
            ecrire_fichier(f, c);            
        }
        terminer_fichier(f);   
    }
    else if (action == "décoder")
    {
        
        cout << "Quel est le nom du fichier ?  ";
        string mot;
        getline(cin, mot);
        cout << endl;
        ifstream f(mot + ".wav", ios::binary);
        if (f.is_open())
        {
            string mot;
            mot = lire_fichier(f);
            cout << "Le texte décodé est :  " << mot <<endl;              
            f.close();
        }
        else cout << "Unable to open file"; 
    }
    
  return 0;
}