#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;

struct Stats {
    double mean;
    double variance;
    double stddev;
};
vector<double> computeErrors( vector<double>a, vector<double> b){
    vector<double> errors;
    for(int i=0; i<a.size(); i++){
        errors.push_back(a[i]-b[i]);
    }
    return errors;
}


Stats calculateStats(vector<double> errors){
    double mean=0,variance=0,stddev=0;
    double n=errors.size();
    for (double i:errors){
        mean=mean+i;
    }
    mean=mean/n;
    for (double i:errors){
        variance=variance+(i-mean)*(i-mean);
    }
    variance=variance/n;
    stddev=sqrt(variance);
    return {mean, variance, stddev};
}

class generator{
public:
    vector<double> samples;
    int f;
    int Fs;
    int n;
    generator(int Fs_, int f_, int n_) : Fs(Fs_),f(f_),n(n_)
    {
    samples.resize(n);
    for (int i=0; i<n; i++){
        samples[i]=sin(2*M_PI*i*f/Fs);
        }
    }
};

vector<int16_t> quantize(const vector<double> samples){
    vector<int16_t> samples_int;
    for (double x : samples){
        samples_int.push_back(static_cast<int16_t>(round(x*32767)));
    }
    return samples_int;
}
vector<double> dequantize(const vector<int16_t> samples){
    vector<double> samples_double;
    for (int x : samples){
        samples_double.push_back(static_cast<double>(x/32767.0));
    }
    return samples_double;
}


vector<double> interpolate(const vector<double> samples){
    vector<double> samples_interp;
    samples_interp.push_back(samples[0]);
    for( int i=1; i<samples.size(); i++){
        samples_interp.push_back((samples[i-1]+samples[i])/2);
        samples_interp.push_back(samples[i]);
    }
    return samples_interp;
};

vector<int16_t> interpolate_int(const vector<int16_t> samples){
    vector<int16_t> samples_interp;
    samples_interp.push_back(samples[0]);
    for( int i=1; i<samples.size(); i++){
        samples_interp.push_back((samples[i-1]+samples[i])/2);
        samples_interp.push_back(samples[i]);
    }
    return samples_interp;
};

int main(){
    ofstream file("output.csv");
    file << "f,mean_quant,stddev_quant,mean_interp,stddev_interp,mean_interpint,stddev_interpint\n";
    for (int f =0; f<51; f++){
    //семлы с генератора
    generator gen(100,f,501); // исходный
    generator test(200,f,1001); //для проверки интерполяции
    //квантование
    vector<int16_t> samples_int= quantize(gen.samples);  //исходное
    vector<double> samples_double=dequantize(samples_int); //обратное квантование
    //интерполяция
    vector<int16_t> samples_interp_int=interpolate_int(samples_int);
    vector<double> samples_interp = interpolate(gen.samples);
    vector<double>samples_interp_deq= dequantize(samples_interp_int);
    //ошибки
    Stats statsquant = calculateStats(computeErrors(samples_double,gen.samples));
    Stats statsinterp = calculateStats(computeErrors(samples_interp,test.samples));
    Stats statsinterpint = calculateStats(computeErrors(samples_interp_deq,test.samples));
    file << f << "," << statsquant.mean << "," << statsquant.stddev << "," 
     << statsinterp.mean << "," << statsinterp.stddev << ","<< statsinterpint.mean << ","<< statsinterpint.stddev << "\n";
    }
    file.close();
};