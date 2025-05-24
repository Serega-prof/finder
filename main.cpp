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
    //������ ������� ����������������
    float sensitivity;
    //��������� ���������������� ������ 
    sensitivity = 3;
    cout << "������� ����������������: " << sensitivity << " �� " << endl;

    //������ ������ �� �����
    string filename = "Sliplis_melkie.pcm"; //��� ����� ��� ������    
    vector<float> samples = open_file(filename);

    //�������� �� ���� �� ������ �������
    if (samples.empty())
    {
        return 1;
    }

    size_t num_samples = samples.size(); //����� ������� � �����

    vector <float> sig_sm(num_samples, -160);
    sig_sm = single_smooth(samples, sensitivity, 10, sig_sm);
    vector <float> sig_sm100 = single_smooth100(samples, sensitivity, 100, sig_sm);

    float a = 0;
    float b = 0;
    size_t c = 0;

    for (int i = 0; i < num_samples; i++) // ���� ��� ����������� ������ � �����, ��� ����� ��������� ������� ������� 
    {
        if (sig_sm100[i] == 1)
        {
            sig_sm100[i] = samples[i];
        }
    }

    // ���������� �������    
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