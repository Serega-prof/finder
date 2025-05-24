#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "..\finder\Header.h" 

#include "matplotlibcpp.h" 

using namespace std;
namespace plt = matplotlibcpp;

int main()
{
    setlocale(LC_ALL, "ru");
    //Вводим уровень чувствительности
    float sensitivity;
    //Временная чувствительность задана 
    sensitivity = 3;
    cout << "Выбрана чувствительность: " << sensitivity << " дБ " << endl;

    //Чтение данных из файла
    string filename = "Sliplis_melkie.pcm"; //Имя файла для чтения    
    vector<float> samples = open_file(filename);

    //Проверка не пуст ли массив семплов
    if (samples.empty())
    {
        return 1;
    }

    size_t num_samples = samples.size(); //Число семплов в файле

    vector <float> sig_sm(num_samples, -160);
    sig_sm = single_smooth(samples, sensitivity, 10, sig_sm);
    vector <float> sig_sm100 = single_smooth100(samples, sensitivity, 100, sig_sm);

    float a = 0;
    float b = 0;
    size_t c = 0;

    for (int i = 0; i < num_samples; i++) // Цикл для копирования данных в месте, где нашли одиночные плавные сигналы 
    {
        if (sig_sm100[i] == 1)
        {
            sig_sm100[i] = samples[i];
        }
    }

    // Построение графика    
    vector<float> x_axis;

    for (int i = 0; i < num_samples; i++)
    {
        x_axis.push_back(i);
    }

    plt::plot(x_axis, samples, "y-");
    plt::plot(x_axis, sig_sm100, "m--");     
    plt::title("Signal spectrum");
    plt::xlabel("Samples");
    plt::ylabel("Amplitude, dB/mW");
    plt::show();

    return 0;
}