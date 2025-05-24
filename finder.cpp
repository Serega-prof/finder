#include "Header.h" 
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

size_t b3 = 0; 
using namespace std;

//������� �������� � ������ pcm ����� (�� ���� ���� � �����)
vector<float> open_file(const string& filename)
{
    // �������� ����� 
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "�� ������� ������� ����." << endl;
    }

    // ������ ������ �� �����
    vector<float> samples;
    float sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(float))) //��������� 32 ���� (������ float) �� ������ ���������� sample, �� ���������, ��� �� ������� char, � �� float �. �. ������� ������� char 
    {
        samples.push_back(sample); //��������� �������� sample � ����� ������� samples
    }
    file.close();

    return samples;
}

//������� ���������� �������� �������� ������ ��������� �� �������� �� gap �������
vector<float> mid_gap(const vector<float>& samples, int small_gap)
{
    vector <float> mid_val_gap; //������, ��� ����� ������� ������� �������� �� ������� gap
    float sum_gap_samples = 0;
    int temp = 0;
    size_t num_samples = samples.size(); //����� ������� � �����    

    for (int i = 0; i < num_samples; i += small_gap) //����, ������� ������� ������� ��������  ��������� �� gap �������� 
    {
        sum_gap_samples = 0;

        for (int j = i; j < i + small_gap; j++)//���������� gap �������� 
        {
            sum_gap_samples += samples[j];
        }

        mid_val_gap.push_back(sum_gap_samples / small_gap); //�������� ������� �������� �� gap �������� � ��������� ��� � ������ 
    }

    return mid_val_gap;
}

//������� ���������� �������� �������� ������ ��������� �� �������� �� gap ������� �� ������� �� small_gap. ��������, ������� ������� �� 100 ��������, ������� � 0 �� 100, ����� � 10 �� 110, � 20 �� 120 � �.�.
vector<float> mid_gap_shift(const vector<float>& samples, int small_gap, int big_gap)
{
    if (big_gap > small_gap)
    {
        int temp = 0;
        size_t num_samples = samples.size(); //����� ������� � �����  
        vector <float> mid_gap_sh; //������, ��� ����� ������� ������� �������� �� ������� gap 
        float sum_gap_samples = 0;

        for (int i = 0; i <= num_samples - big_gap; i += small_gap) //����, ������� ������� ������� ��������  ��������� �� gap ��������
        {
            sum_gap_samples = 0;

            for (int j = i; j < i + big_gap; j++)//���������� big_gap ��������   
            {
                sum_gap_samples += samples[j];
            }

            mid_gap_sh.push_back(sum_gap_samples / big_gap); //�������� ������� �������� �� gap �������� � ��������� ��� � ������
            temp = i;
        }

        return mid_gap_sh;
    }

    else
    {
        cout << "������. ������� ������� ������ ��� ����� �������� �������!" << endl;
    }

}

//������� ������������� ����������������� �����������
vector <float> symmetric(const vector<float>& input, float alpha)
{
    if (input.empty()) return {};

    vector<float> filtered = input; // �������� �������� ������

    // ������ ������ (����� �������)
    for (size_t i = 1; i < filtered.size(); i++)
    {
        filtered[i] = alpha * input[i] + (1 - alpha) * filtered[i - 1];
    }

    // �������� ������ (������ ������) 
    for (int i = filtered.size() - 2; i >= 0; i--) {
        filtered[i] = alpha * filtered[i] + (1 - alpha) * filtered[i + 1];
    }

    return filtered;
}

//������� ��� ���������� ���������� ����������� ��������
vector <float> single_smooth2(const vector<float>& samples, float sensitivity, int gap)
{
    size_t num_samples = samples.size(); //����� ������� � �����  
    vector <float> sig_sm(num_samples, -200);
    vector <float> temp;
    vector <float> expo; //������, �������� ������ ����� ����������� (����� �� ����� ��� � � samples) 
    float n = 0;

    expo = symmetric(samples, 0.08);
    //������� ������, ����� ������� �� small_gap ��� �������
    expo.erase(expo.end() - (num_samples % gap), expo.end());

    temp = mid_gap(expo, gap); //����� ������ �� ������� �� gap ������� � ������� ������� �������� �� ���� ��������

    //���� ��� ��������� ������� �� �������� ����������
    /*�.�.������� �������� ������, ��� �������� �������, �� ���������� ������
    ������� �������� ���, ����� �� ������� ��������� ������� �������� � �� ����� ������� ��� �������*/
    for (int i = 0; i < temp.size(); i++)
    {
        for (int j = i * gap; j < (i + 1) * gap; j++)
        {
            sig_sm[j] = temp[i];
        }
    }

    return sig_sm;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//������� ���������� ������ �������� ������ ���������
float calc_general_middle(const vector<float>& samples, size_t num_samples)
{
    double sum_all_samples = 0;
    float general_middle;

    for (int i = 0; i < num_samples; i++)
    {
        sum_all_samples += samples[i];

        if (i == (num_samples - 1))
        {
            general_middle = sum_all_samples / num_samples;

            return general_middle;
        }
    }
}

//������� ��� ��������� ���������� �������� �������

vector <float> single_smooth(const vector<float>& samples, float sensitivity, int gap, vector<float>& sig_sm)
{
    size_t num_samples = samples.size(); //����� ������� � �����  
    //vector <float> sig_sm(num_samples, -200);
    vector <float> temp;
    vector <float> temp2;//������, �������� ������� �������� ������� expo ���, ��� ��� ���� �����
    vector <float> expo; //������, �������� ������ ����� ����������� (����� �� ����� ��� � � samples)
    float n = 0;
    vector <float> onehun(num_samples, -2);
    expo = symmetric(samples, 0.08);

    //������� ������, ����� ������� �� small_gap ��� �������
    expo.erase(expo.end() - (num_samples % gap), expo.end());

    temp = mid_gap(expo, gap); //����� ������ �� ������� �� gap ������� � ������� ������� �������� �� ���� ��������

    //���� ��� ��������� ������� �� �������� ����������

    temp2 = mid_gap_shift(expo, gap, 10 * gap);

    sig_sm = comparison(samples, temp, temp2, gap, gap * 10, sensitivity);

    return sig_sm;

}

vector <float> single_smooth100(const vector<float>& samples, float sensitivity, int gap, const vector<float>& prev)
{
    size_t num_samples = samples.size(); //����� ������� � �����  
    vector <float> sig_sm(num_samples, -200);
    vector <float> temp;
    vector <float> temp2;//������, �������� ������� �������� ������� expo ���, ��� ��� ���� �����
    vector <float> expo; //������, �������� ������ ����� ����������� (����� �� ����� ��� � � samples)
    float n = 0;
    vector <float> onehun(num_samples, -2);
    expo = symmetric(samples, 0.08);

    //������� ������, ����� ������� �� small_gap ��� �������
    expo.erase(expo.end() - (num_samples % gap), expo.end());

    temp = mid_gap(expo, gap); //����� ������ �� ������� �� gap ������� � ������� ������� �������� �� ���� ��������

    //���� ��� ��������� ������� �� �������� ����������
    /* �.�.������� �������� ������, ��� �������� �������, �� ���������� ������
    ������� �������� ���, ����� �� ������� ��������� ������� �������� � �� ����� ������� ��� �������*/

    temp2 = mid_gap_shift(expo, gap, 10 * gap);


    onehun = comparison100(samples, prev, temp, temp2, gap, gap * 10, sensitivity);
    for (int i = 0; i < num_samples; i++)
    {
        if (prev[i] == 1)
            //if (onehun[i] == 1)
        {
            sig_sm[i] = 1;
        }
    }
    for (int i = 0; i < num_samples; i++)
    {
        //if (prev[i] == 0 and onehun[i] == 1)
        if (onehun[i] == 1)
        {
            sig_sm[i] = 1;
        }
    }

    return sig_sm;
}

//������� ��������� �������� ������ ��������� ������� � ������ �������� ������, ��� ����������� ���� �� � ����� � ������� � ���� ��� ��� ������ ������/������ ���
int find_general_differences(const vector<float>& mid_val_gap, float general_middle, float level_change, bool sens)
{
    int k = 0;
    size_t num_mid_val = mid_val_gap.size(); //����� ������, �� ������� ������� ���� ����

    if (!sens) //���� �������������� ����������������
    {
        for (int i = 0; i < num_mid_val; i++)
        {
            if (mid_val_gap[i] > general_middle + level_change or mid_val_gap[i] < general_middle - level_change)
            {
                k++;
            }
        }
    }

    else //���� ������������ ����������������
    {
        if (general_middle < 0) // ���� ����� ������� �������� ������ ����
        {
            for (int i = 0; i < num_mid_val; i++)
            {
                if (mid_val_gap[i] < general_middle + (general_middle * level_change) or mid_val_gap[i] > general_middle - (general_middle * level_change))
                {
                    k++;
                }
            }
        }
        //���� general_middle == 0, �� ����� ��������� �� �������� ������ ������, ��� ��� ���� ������

        else
        {
            for (int i = 0; i < num_mid_val; i++)
            {
                if (mid_val_gap[i] > general_middle + (general_middle * level_change) or mid_val_gap[i] < general_middle - (general_middle * level_change))
                {
                    k++;
                }
            }
        }
    }

    //��������� � ������� �������
    if (k > 0)
    {
        cout << "� ���� ����� ���� � ������� � ����!" << endl;

        return 0;
    }
    //��������� �� ������. ������ �� �������
    else
    {
        cout << "� ���� ����� ���� ������ �������, ���� ������ ����. ���������� �������� ������� ����������������!" << endl;

        return 1;
    }
}

vector<float> comparison(const vector<float>& samples, const vector<float>& mid_gap, const vector<float>& mid_gap_sh, int small_gap, int big_gap, float sensitivity)
{
    size_t num_samples = samples.size();//����� ������� � �����  
    vector <float> suspicion(num_samples, -200);//�������� ������
    size_t num_mid_gap_sh = mid_gap_sh.size();
    size_t num_mid_gap = mid_gap.size();
    int shift = 6; //��������, ���������� �� ������� �������� ��������� ��� �������� �� �����-�������� �������
    size_t val = big_gap / small_gap + shift;
    int front, fall, check_shift = 0, repeat = 0;
    float max_val = -1000000; // ���� �������� ������� �������
    //float times = pow(10, (sensitivity / 10)); //� times ��� ������ ���� �������, ����� �� ������� ��� �������� 
    int p; // ���������� �� �������������
    int w_save = 0;//���������� ������ ��� ���������� �������
    //cout << "num_mid_gap_sh: " << num_mid_gap_sh << endl;
    //cout << "num_mid_gap: " << num_mid_gap << endl;
    int b0;
    int b1 = 0; //�������� ���������� �� ���������� ��������� ��������
    int b2 = 0; //�������� ���������� �� ����� shift ��� �������
    //int b3 = 0; //�������� ��������� �������. ���������� ������� ����������
    float b4 = 0; //�������� ��� �������� ����� ��������� 
    int lenght1 = 0; //����� ������� 
    int b5 = 0; //�������� ��� ������ ������ �������������� �������
    int b6 = 0; //�������� ��� ����������� ��������� �������
    float b7 = 0; //�������� ��� ���������� �������� ����� ���� � ��������� ���������
    for (size_t i = 0; i < num_mid_gap_sh; i++)
    {
        //b1 = 0;
        max_val = -1000000;
        p = 0;
        lenght1 = 0;
        b7 = 0;

        if (val + i < num_mid_gap) //���� ���� �������� ������� ������� �������, �� �� ��� ���������  
        {
            max_val = -1000000;

            //���� ���� ������� �� ������� ����������������
            if (mid_gap[val + i] > (mid_gap_sh[i] + sensitivity))// and mid_gap[val + i] < (mid_gap_sh[i] + 4 * sensitivity))
            {
                front = 0;

                //���������, ����� ��������������� ���������� ��� � ��������
                for (int j = 0; j < shift + 1; j++)
                {
                    if (mid_gap[val + i - shift + j] < mid_gap[val + i - shift + j + 1])
                    {
                        front++;
                    }
                }

                //������� ������
                if (front >= shift)
                {
                    b3++;
                    //b1++;
                    check_shift = 0;
                    //���� �� �������, �� ���������� ��� ������ �������
                    for (size_t d = (val + i - shift) * small_gap; d < num_samples; d++)
                    {
                        if (d == ((val - shift + i) * small_gap))
                        {
                            cout << "��������� ������ ����� " << b3 << " ������� �� ��������: " << mid_gap[val - shift + i] << endl;
                        }
                        fall = 0;
                        //������� ��������� ���� �� ������ ��� ���������
                        if (val + i + check_shift + shift < num_mid_gap)
                        {
                            //����� ������ ������������ �������� ����� ����� �� ���� �������
                            if (max_val < mid_gap[val + i + check_shift])
                            {
                                max_val = mid_gap[val + i + check_shift];
                                b7 = max_val - mid_gap[val + i - shift];
                            }
                            //������ 5 ����� �������� ����������� �������, ����� ����� ������ ��� �������� ����� ��������� ���� �� ��� ����� �������
                            if (d >= (val + i) * small_gap and d % small_gap == 0)
                            {
                                check_shift++;
                                // ���� ����. �� ���� �������� �� ������� ���� ��� �� ����� ���������� �� ������� ����������������
                                if (max_val - sensitivity > mid_gap[val + i + check_shift])
                                {
                                    //������� ���� �� �� ��� ���������� ��������
                                    for (int j = 0; j < shift + 1; j++)
                                    {
                                        if (mid_gap[val + i + check_shift + j] > mid_gap[val + i + check_shift + j + 1])
                                        {
                                            fall++;
                                        }
                                    }

                                    //�� ������ ����������
                                    if (fall == shift - 2)
                                    {
                                        //��������, ��� ���� ����� 15 ������� ���� �������, ������� ���� ���� �� �� 1 ��, �� �� ����������� ������, �� ��������� ��������� ����
                                        if (!(mid_gap[val + i + check_shift] - 1 > mid_gap[val + i + check_shift + 3 * shift]))
                                        {

                                            //��������� ������� �� �� �����
                                            for (int r = 0; r < shift; r++)
                                            {
                                                //0.3 ��������� ��� ����, ����� ���������� �������� ��������� ���� ���� �� 0.3 ��, ����� ����� ����������� �������� ������ ������
                                                if (mid_gap[val + i + check_shift + r] - 0.3 > mid_gap[val + i + check_shift + r + 1])
                                                {
                                                    b6++;
                                                }
                                            }

                                            if (b6 != 0)
                                            {
                                                b6 += 2; //����� ������� ������ ���� ��-�� ������������ ���������
                                                for (int r = 0; r < b6; r++)
                                                {
                                                    for (int r1 = 0; r1 < small_gap * (r + 1); r1++)
                                                    {
                                                        suspicion[d + r1] = 1;
                                                    }
                                                }
                                            }

                                            fall = 0;
                                            // -1 ������ ��-�� ����, ��� check_shift ��� ����������� ��� ������ � ����� ��������, �� � ��� �������� �� ������ � �������� �� ����������. � ��� -1, ����� ������ ����� ��������� �������� �� ����� ����

                                            b4 = d / small_gap - 1 + b6;
                                            cout << "��������� ������ �����: " << b3 << " ����������, d: " << mid_gap[b4] << endl;
                                            cout << "-----------------------------------------------------" << endl;

                                            lenght1 = b4 - val - i + shift;
                                            b6 = 0;
                                            break;
                                        }

                                        else
                                        {
                                            suspicion[d] = 1;
                                        }
                                    }

                                    else
                                    {
                                        suspicion[d] = 1;
                                    }
                                }

                                else
                                {
                                    suspicion[d] = 1;
                                }
                            }

                            else
                            {
                                suspicion[d] = 1;
                            }
                        }
                    }


                    //���� ��� ������ ���������� ������������� ��������
                    for (int s = 0; s < num_mid_gap; s++)
                    {
                        b1++;
                        max_val = -1000000;

                        //������ ������������� ������ ��� ���. ���� ����� ��� ��������, ���� � ��� �� shift (�����) ������ ����� ����������
                        for (int t = 0; t < 4 * shift; t++)
                        {
                            //val + i + check_shift - 2 - ��� �������� �� ������� ���������� ������ (��������� ���������� � �������������� ������), ����� ��������� ������� � ������ �� ������� ������ �.� big_gap/small_gap = 10, ���� �� ����, �� ����� ������� ��������
                            //�������� �� �������. ������� ����� �� ������ ���� ����� ������� �.�. �� ���� �� ������ ������� �����

                            float l = mid_gap[b4] + sensitivity;
                            float l1 = mid_gap[b4 + shift + t];

                            if (l < l1)
                            {
                                for (int r = 0; r < shift + t; r++)///////////////////////////////////////////////////////////////////////������ ������� ����� ��������
                                {
                                    if (mid_gap[b4 + r] < mid_gap[b4 + r + 1])
                                    {
                                        repeat++;
                                    }
                                }

                            }

                            if (repeat >= shift)
                            {
                                //���������� �������� t, ����� ����� �� ������� ��������
                                b0 = t;

                                //�������� �� ������� �� ������ ������
                                for (int t1 = 0; t1 < t; t1++)
                                {
                                    for (int r = 0; r < shift; r++)
                                    {
                                        if (mid_gap[b4 + t1 + r] < mid_gap[b4 + t1 + r + 1])
                                        {
                                            b5++;
                                        }
                                    }

                                    if (b5 >= shift - 1)
                                    {
                                        b0 = t1 + 2;
                                        b5 = 0;
                                        break;
                                    }

                                    b5 = 0;
                                }

                                break;
                            }

                            else
                            {
                                repeat = 0;
                            }
                        }

                        if (repeat < shift)
                        {
                            cout << "������������� ������ �� ���������" << endl;
                            //����� ������ �� ��������� �� ��������, ������ ������� ��������
                            i += (lenght1);
                            repeat = 0;
                            b1 = 0;
                            b2 = 0;

                            lenght1 = 0;
                            break;
                        }

                        else
                        {
                            if (b0 == 0)
                            {
                                b0 += 2;
                            }

                            check_shift = 0; //�.�. ����� ����� ������, �� � ���������� ������ ����������
                            b3++; //������� ����� �������
                            repeat = 0;
                            int f1 = b4 + b0;
                            cout << "������������� ������ �����: " << b3 << " ������� �� ��������: " << mid_gap[f1] << endl;
                            //������ �������� ���� ������ ���� ����� ������. ������ - 1 � d, ����� ���� ������� ��������� ���� ���� �� �������, ��� ���� �������� ���� �� �����
                            for (size_t d = f1 * small_gap; d < num_samples; d++)
                            {
                                fall = 0;
                                //������� ��������� ���� �� ������ ��� ���������
                                if (b4 + b0 + check_shift + shift < num_mid_gap)
                                {
                                    //����� ������ ������������ �������� ����� ����� �� ���� �������
                                    if (max_val < mid_gap[b4 + b0 + check_shift + shift])
                                    {
                                        max_val = mid_gap[b4 + b0 + check_shift + shift];
                                    }
                                    //������ 5 ����� �������� ����������� �������, ����� ����� ������ ��� �������� ����� ��������� ���� �� ��� ����� �������
                                    if (d >= (b4 + b0 + shift) * small_gap and d % small_gap == 0)
                                    {
                                        check_shift++;
                                        // ���� ����. �� ���� �������� �� ������� ���� ��� �� ����� ���������� �� ������� ����������������
                                        float now = mid_gap[b4 + b0 + check_shift + shift];
                                        if (max_val - sensitivity > mid_gap[b4 + b0 + check_shift + shift])
                                        {
                                            //������� ���� �� �� ��� ���������� ��������
                                            for (int j = 0; j < shift + 1; j++)
                                            {
                                                if (mid_gap[b4 + b0 + check_shift + shift + j] > mid_gap[b4 + b0 + check_shift + shift + j + 1])
                                                {
                                                    fall++;
                                                }
                                            }

                                            //���� �� ����, �� ������ ����������
                                            if (fall <= shift - 2)
                                            {
                                                //��������, ��� ���� ����� 15 ������� ���� �������, ������� ���� ���� �� �� 1 ��, �� �� ����������� ������, �� ��������� ��������� �����
                                                if (!(mid_gap[b4 + b0 + check_shift + shift] - 1 > mid_gap[b4 + b0 + check_shift + 4 * shift]))
                                                {
                                                    fall = 0;
                                                    b2++;

                                                    for (int r = 0; r < shift; r++)
                                                    {
                                                        if (mid_gap[val + i + check_shift + r] > mid_gap[val + i + check_shift + r + 1])
                                                        {
                                                            b6++;
                                                        }
                                                    }

                                                    if (b6 != 0)
                                                    {
                                                        b6 += 2;
                                                        for (int r = 0; r < b6; r++)
                                                        {
                                                            for (int r1 = 0; r1 < small_gap * (r + 1); r1++)
                                                            {
                                                                suspicion[d + r1] = 1;
                                                            }
                                                        }
                                                    }
                                                    lenght1 += d / small_gap + b6 - 1 - b4 - b0;
                                                    b4 = d / small_gap - 1 + b6;
                                                    b6 = 0;
                                                    b0 = 0;
                                                    cout << "������������� ������ �����: " << b3 << " ���������� ���������: " << mid_gap[b4] << endl;
                                                    cout << "________________________________________________________________" << endl;
                                                    break;
                                                }

                                                else
                                                {
                                                    suspicion[d] = 1;
                                                }
                                            }

                                            else
                                            {
                                                suspicion[d] = 1;
                                            }


                                        }

                                        else
                                        {
                                            suspicion[d] = 1;
                                        }
                                    }

                                    else
                                    {
                                        suspicion[d] = 1;
                                    }
                                }
                            }
                        }
                    }
                    //������� ����� ����� �������������� �������
                //������ ���� BREAK?\, �� ����������!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //break;
                }
            }
        }

    }

    return suspicion;
}

//������� ��� ��������� ��� �������� ����
vector<float> comparison100(const vector<float>& samples, const vector<float>& sig_sm, const vector<float>& mid_gap, const vector<float>& mid_gap_sh, int small_gap, int big_gap, float sensitivity)
{
    size_t num_samples = samples.size();//����� ������� � �����  
    vector <float> suspicion(num_samples, -200);//�������� ������
    size_t num_mid_gap_sh = mid_gap_sh.size();
    size_t num_mid_gap = mid_gap.size();
    int shift = 5; //��������, ���������� �� ������� �������� ��������� ��� �������� �� �����-�������� �������
    size_t val = big_gap / small_gap + shift;
    int front, fall, check_shift = 0, repeat = 0;
    float max_val = -1000000; // ���� �������� ������� �������
    //float times = pow(10, (sensitivity / 10)); //� times ��� ������ ���� �������, ����� �� ������� ��� �������� 
    int p; // ���������� �� �������������
    int w_save = 0;//���������� ������ ��� ���������� �������
    //cout << "num_mid_gap_sh: " << num_mid_gap_sh << endl;
    //cout << "num_mid_gap: " << num_mid_gap << endl;
    int b0;
    int b1 = 0; //�������� ���������� �� ���������� ��������� ��������
    int b2 = 0; //�������� ���������� �� ����� shift ��� �������
    //int b3 = 0; //�������� ��������� �������. ������� ����������
    float b4 = 0; //�������� ��� �������� ����� ��������� 
    int lenght1 = 0; //����� ������� 
    int b5 = 0; //�������� ��� ������ ������ �������������� �������
    int b6 = 0; //�������� ��� ����������� ��������� �������
    float b7 = 0; //�������� ��� ���������� �������� ����� ���� � ��������� ���������
    int b8 = 0; //�������� ��� ���������� ������ �������, ����, ��� ��� ��� ������� �������
    for (size_t i = 0; i < num_mid_gap_sh; i++)
    {

        max_val = -1000000;
        p = 0;
        lenght1 = 0;
        b7 = 0;

        if (val + i < num_mid_gap) //���� ���� �������� ������� ������� �������, �� �� ��� ���������  
        {
            max_val = -1000000;

            //���� ���� ������� �� ������� ����������������
            if (mid_gap[val + i] > (mid_gap_sh[i] + sensitivity))// and mid_gap[val + i] < (mid_gap_sh[i] + 4 * sensitivity))
            {
                front = 0;

                //���������, ����� ��������������� ���������� ��� � ��������
                for (int j = 0; j < shift + 1; j++)
                {
                    if (mid_gap[val + i - shift + j] < mid_gap[val + i - shift + j + 1])
                    {
                        front++;
                    }
                }
                b8 = 0;

                //for (int y1 = 0; y1 < big_gap; y1++) 
                //{
                //    //if (val + i - shift + y1 < num_samples) 
                //    //{
                //        if (sig_sm[(val + i - shift) * small_gap + y1] != -160)   
                //        {
                //            b8++; 
                //        }
                //    //}
                //}
                //������� ������
                if (front >= shift and b8 == 0)
                {


                    //b1++;
                    check_shift = 0;
                    //���� �� �������, �� ���������� ��� ������ �������
                    for (size_t d = (val + i - shift) * small_gap; d < num_samples; d++)
                    {
                        if (d == ((val - shift + i) * small_gap))
                        {
                            b8 = 0;

                            for (int y1 = 0; y1 < shift * big_gap; y1++)
                            {
                                if (sig_sm[d + y1] == 1)
                                {
                                    b8++;
                                }
                            }

                            if (b8 != 0)
                            {
                                b8 = 0;
                                break;
                            }
                            b3++;
                            cout << "��������� ������ ����� " << b3 << " ������� �� ��������: " << mid_gap[val - shift + i] << endl;
                        }
                        fall = 0;
                        //������� ��������� ���� �� ������ ��� ���������
                        if (val + i + check_shift + shift < num_mid_gap)
                        {
                            //����� ������ ������������ �������� ����� ����� �� ���� �������
                            if (max_val < mid_gap[val + i + check_shift])
                            {
                                max_val = mid_gap[val + i + check_shift];
                                b7 = max_val - mid_gap[val + i - shift];
                            }
                            //������ 5 ����� �������� ����������� �������, ����� ����� ������ ��� �������� ����� ��������� ���� �� ��� ����� �������
                            if (d >= (val + i) * small_gap and d % small_gap == 0)
                            {
                                check_shift++;
                                // ���� ����. �� ���� �������� �� ������� ���� ��� �� ����� ���������� �� ������� ����������������
                                if (max_val - sensitivity > mid_gap[val + i + check_shift])
                                {
                                    //������� ���� �� �� ��� ���������� ��������
                                    for (int j = 0; j < shift + 1; j++)
                                    {
                                        //- 0.2 ��� ����, ����� ��� ��������� ������� ����������� ���������, ������� ��� ���������� ����� ������� ����� �� ��������� � �� ������� ��������� � 0.2 �� �����������
                                        if (mid_gap[val + i + check_shift + j] - 0.2 > mid_gap[val + i + check_shift + j + 1])
                                        {
                                            fall++;
                                        }
                                    }
                                    //�������� ��� ������� ��������, ��� �� ������ ��������� �� ���������� ���� ��� �� 50%
                                    if ((b7 * 0.4) < (max_val - mid_gap[val + i + check_shift]))
                                    {
                                        //�� ������ ����������
                                        if (fall <= shift - 2)
                                        {
                                            //��������, ��� ���� ����� 15 ������� ���� �������, ������� ���� ���� �� �� 1 ��, �� �� ����������� ������, �� ��������� ��������� �����
                                            if (!(mid_gap[val + i + check_shift] - 1 > mid_gap[val + i + check_shift + 3 * shift]))
                                            {

                                                //��������� ������� �� �� �����
                                                for (int r = 0; r < shift; r++)
                                                {
                                                    if (mid_gap[val + i + check_shift + r] > mid_gap[val + i + check_shift + r + 1])
                                                    {
                                                        b6++;
                                                    }
                                                }

                                                if (b6 != 0)
                                                {
                                                    b6 += 2; //����� ������� ������ ���� ��-�� ������������ ���������
                                                    for (int r = 0; r < b6; r++)
                                                    {
                                                        for (int r1 = 0; r1 < small_gap * (r + 1); r1++)
                                                        {
                                                            suspicion[d + r1] = 1;
                                                        }
                                                    }
                                                }

                                                fall = 0;
                                                // -1 ������ ��-�� ����, ��� check_shift ��� ����������� ��� ������ � ����� ��������, �� � ��� �������� �� ������ � �������� �� ����������. � ��� -1, ����� ������ ����� ��������� �������� �� ����� ����

                                                b4 = d / small_gap - 1 + b6;
                                                cout << "��������� ������ �����: " << b3 << " ����������: " << mid_gap[b4] << endl;
                                                cout << "-----------------------------------------------------" << endl;

                                                lenght1 = b4 - val - i + shift;
                                                b6 = 0;
                                                break;

                                            }

                                            else
                                            {
                                                suspicion[d] = 1;
                                            }
                                        }

                                        else
                                        {
                                            suspicion[d] = 1;
                                        }
                                    }

                                    else
                                    {
                                        suspicion[d] = 1;
                                    }
                                }

                                else
                                {
                                    suspicion[d] = 1;
                                }
                            }

                            else
                            {
                                suspicion[d] = 1;
                            }
                        }
                    }


                    //���� ��� ������ ���������� ������������� ��������
                    for (int s = 0; s < num_mid_gap; s++)
                    {
                        if (b8 != 0)
                        {

                            break;
                        }
                        b1++;
                        max_val = -1000000;


                        //������ ������������� ������ ��� ���. ���� ����� ��� ��������, ���� � ��� �� shift (�����) ������ ����� ����������
                        for (int t = 0; t < 4 * shift; t++)
                        {

                            //val + i + check_shift - 2 - ��� �������� �� ������� ���������� ������ (��������� ���������� � �������������� ������), ����� ��������� ������� � ������ �� ������� ������ �.� big_gap/small_gap = 10, ���� �� ����, �� ����� ������� ��������
                            //�������� �� �������. ������� ����� �� ������ ���� ����� ������� �.�. �� ���� �� ������ ������� �����

                            float l = mid_gap[b4] + sensitivity;
                            float l1 = mid_gap[b4 + shift + t];

                            if (l < l1)
                            {
                                for (int r = 0; r < shift + t; r++)///////////////////////////////////////////////////////////////////////������ ������� ����� ��������
                                {
                                    if (mid_gap[b4 + r] < mid_gap[b4 + r + 1])
                                    {
                                        repeat++;
                                    }
                                }

                            }

                            if (repeat >= shift)
                            {
                                //���������� �������� t, ����� ����� �� ������� ��������
                                b0 = t;

                                //�������� �� ������� �� ������ ������
                                for (int t1 = 0; t1 < t; t1++)
                                {
                                    for (int r = 0; r < shift; r++)
                                    {
                                        if (mid_gap[b4 + t1 + r] < mid_gap[b4 + t1 + r + 1])
                                        {
                                            b5++;
                                        }
                                    }

                                    if (b5 >= shift - 1)
                                    {
                                        b0 = t1 + 2;
                                        b5 = 0;
                                        break;
                                    }

                                    b5 = 0;
                                }

                                break;
                            }

                            else
                            {
                                repeat = 0;
                            }
                        }

                        if (repeat < shift)
                        {
                            cout << "������������� ������ �� ��������� � b3: " << b3 << endl;
                            //����� ������ �� ��������� �� ��������, ������ ������� ��������
                            i += (lenght1);
                            repeat = 0;
                            b1 = 0;
                            b2 = 0;

                            lenght1 = 0;
                            break;
                        }

                        else
                        {


                            if (b0 == 0)
                            {
                                b0 += 2;
                            }

                            check_shift = 0; //�.�. ����� ����� ������, �� � ���������� ������ ����������
                            b3++; //������� ����� �������
                            repeat = 0;
                            int f1 = b4 + b0;
                            cout << "������������� ������ �����: " << b3 << " ������� �� ��������: " << mid_gap[f1] << endl;
                            //������ �������� ���� ������ ���� ����� ������. ������ - 1 � d, ����� ���� ������� ��������� ���� ���� �� �������, ��� ���� �������� ���� �� �����
                            for (size_t d = f1 * small_gap; d < num_samples; d++)
                            {

                                fall = 0;
                                //������� ��������� ���� �� ������ ��� ���������
                                if (b4 + b0 + check_shift + shift < num_mid_gap)
                                {
                                    //����� ������ ������������ �������� ����� ����� �� ���� �������
                                    if (max_val < mid_gap[b4 + b0 + check_shift + shift])
                                    {
                                        max_val = mid_gap[b4 + b0 + check_shift + shift];
                                    }
                                    //������ 5 ����� �������� ����������� �������, ����� ����� ������ ��� �������� ����� ��������� ���� �� ��� ����� �������
                                    if (d >= (b4 + b0 + shift) * small_gap and d % small_gap == 0)
                                    {
                                        check_shift++;
                                        // ���� ����. �� ���� �������� �� ������� ���� ��� �� ����� ���������� �� ������� ����������������
                                        float now = mid_gap[b4 + b0 + check_shift + shift];
                                        if (max_val - sensitivity > mid_gap[b4 + b0 + check_shift + shift])
                                        {
                                            //������� ���� �� �� ��� ���������� ��������
                                            for (int j = 0; j < shift + 1; j++)
                                            {
                                                if (mid_gap[b4 + b0 + check_shift + shift + j] > mid_gap[b4 + b0 + check_shift + shift + j + 1])
                                                {
                                                    fall++;
                                                }
                                            }
                                            //�������� ��� ������� ��������, ��� �� ������ ��������� �� ���������� ���� ��� �� 50%
                                            if ((b7 * 0.4) < (max_val - mid_gap[val + i + check_shift]))
                                            {
                                                //���� �� ����, �� ������ ����������
                                                if (fall <= shift - 2)
                                                {
                                                    //��������, ��� ���� ����� 15 ������� ���� �������, ������� ���� ���� �� �� 1 ��, �� �� ����������� ������, �� ��������� ��������� �����
                                                    if (!(mid_gap[b4 + b0 + check_shift + shift] - 1 > mid_gap[b4 + b0 + check_shift + 4 * shift]))
                                                    {
                                                        fall = 0;
                                                        b2++;

                                                        for (int r = 0; r < shift; r++)
                                                        {
                                                            if (mid_gap[val + i + check_shift + r] > mid_gap[val + i + check_shift + r + 1])
                                                            {
                                                                b6++;
                                                            }
                                                        }

                                                        if (b6 != 0)
                                                        {
                                                            b6 += 2;
                                                            for (int r = 0; r < b6; r++)
                                                            {
                                                                for (int r1 = 0; r1 < small_gap * (r + 1); r1++)
                                                                {
                                                                    suspicion[d + r1] = 1;
                                                                }
                                                            }
                                                        }
                                                        lenght1 += d / small_gap + b6 - 1 - b4 - b0;
                                                        b4 = d / small_gap - 1 + b6;
                                                        b6 = 0;
                                                        b0 = 0;
                                                        cout << "������������� ������ �����: " << b3 << " ���������� ���������: " << mid_gap[b4] << endl;
                                                        cout << "________________________________________________________________" << endl;
                                                        break;
                                                    }

                                                    else
                                                    {
                                                        suspicion[d] = 1;
                                                    }
                                                }

                                                else
                                                {
                                                    suspicion[d] = 1;
                                                }
                                            }

                                            else
                                            {
                                                suspicion[d] = 1;
                                            }

                                        }

                                        else
                                        {
                                            suspicion[d] = 1;
                                        }
                                    }

                                    else
                                    {
                                        suspicion[d] = 1;
                                    }
                                }
                            }
                        }
                    }
                    //������� ����� ����� �������������� �������
                //������ ���� BREAK?\, �� ����������!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //break;


                }
            }
        }
        //    }
        //}
    }

    return suspicion;
}

