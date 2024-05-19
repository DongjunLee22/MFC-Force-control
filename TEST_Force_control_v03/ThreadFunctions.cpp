#include "pch.h"
#include "ThreadFunctions.h"
#include "TEST_Force_control_v03Dlg.h"
#include "UtilityFunctions.h"
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

UINT Thread_force(LPVOID pParam)
{
    CTESTForcecontrolv03Dlg* g_pDlg = (CTESTForcecontrolv03Dlg*)pParam;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-2.0f, 5.0f); // ���� ������ ���� ����

    // Local Variable
    float noise = g_pDlg->noise;
    float F_total = g_pDlg->F_total;
    float Fd = g_pDlg->Fd;
    float TTCA_gma_hat = g_pDlg->TTCA_gma_hat;
    float TTCA_f_des_dot = g_pDlg->TTCA_f_des_dot;
    float F_old = g_pDlg->F_old;
    float F_old_time = g_pDlg->F_old_time;
    float TTCA_alpha = g_pDlg->TTCA_alpha;
    float TTCA_kf = g_pDlg->TTCA_kf;
    float vz_target = g_pDlg->vz_target;
    float var_freq_force = g_pDlg->var_freq_force;
    float TTCA_err_f = g_pDlg->TTCA_err_f;

    // �ֱ� ���� �� ������ ���� ����
    high_resolution_clock::time_point ts, t_start;
    nanoseconds t_duration_ns;
    double t_duration_ns_double = 0.0;
    int i_freq = 0;
    double td_s_sum = 0.0;

    // �ʱ� ���������� ��ġ ��ǥ
    float pos[3] = { g_pDlg->ini_pos_actual[0], g_pDlg->ini_pos_actual[1], g_pDlg->ini_pos_actual[2] };

    const int update_period_ms = 1000 / 16; // 16Hz ������Ʈ �ֱ�
    int update_counter = 0;

    t_start = high_resolution_clock::now();

    while (g_pDlg->m_bRunThread_force)
    {
        // �ֱ� ���� �ð� ���
        ts = high_resolution_clock::now();

        // 20N�� ������ ���� ���� ������ �߰�
        F_total = 20.0;
        noise = dis(gen);
        F_total = F_total + noise;

        // TTCA ����� ȣ��
        float current_time = duration_cast<nanoseconds>(high_resolution_clock::now() - t_start).count() / 1e9f;

        TTCAcontrol(Fd, F_total, TTCA_gma_hat, TTCA_f_des_dot,
            F_old, F_old_time, TTCA_alpha, TTCA_kf,
            current_time, vz_target, TTCA_err_f);

        pos[0] += 1.0;
        pos[2] += 2.0;


        //////////////////////////////////////////////////////////////////////////
        //// �ֱ������� UI ������Ʈ �޽��� ����Ʈ +++++++++++++++++++++++++++
        //////////////////////////////////////////////////////////////////////////
        if (++update_counter >= update_period_ms)
        {
            UIUpdateData* data = new UIUpdateData;
            data->pos[0] = pos[0];
            data->pos[1] = pos[1];
            data->pos[2] = pos[2];
            data->force = F_total;
            data->freq = var_freq_force;
            data->vz_target = vz_target * 0.001;
            data->TTCA_f_des_dot = TTCA_f_des_dot;
            data->TTCA_gma_hat = TTCA_gma_hat;
            data->TTCA_err_f = TTCA_err_f;
            g_pDlg->PostMessage(WM_UPDATE_UI, 0, reinterpret_cast<LPARAM>(data));
            update_counter = 0; // Reset counter
        }

        //////////////////////////////////////////////////////////////////////////
        //// while�� �ӵ� ���� +++++++++++++++++++++++++++++++++++++++++++++++++++
        //////////////////////////////////////////////////////////////////////////
        t_duration_ns_double = 0.0;
        while (t_duration_ns_double < 1000000) // 1 millisecond
        {
            t_duration_ns = high_resolution_clock::now() - ts;
            t_duration_ns_double = double(t_duration_ns.count());
        }

        //////////////////////////////////////////////////////////////////////////
        //// �ֱ� ���� �� ��� +++++++++++++++++++++++++++++++++++++++++++++++++++
        //////////////////////////////////////////////////////////////////////////
        i_freq++;
        td_s_sum += (t_duration_ns_double / 1000000000.0);

        if (i_freq == 1000)
        {
            var_freq_force = 1.0 / (td_s_sum / 1000.0);
            CString strFreq;
            strFreq.Format(_T("%.2f Hz"), var_freq_force);
            g_pDlg->m_var_freq_force.SetWindowTextW(strFreq);

            i_freq = 0;
            td_s_sum = 0.0;
        }
        //////////////////////////////////////////////////////////////////////////
    }

    // ������ ���� ��, ������Ʈ�� ���� �ٽ� g_pDlg�� ����
    g_pDlg->noise = noise;
    g_pDlg->F_total = F_total;
    g_pDlg->TTCA_gma_hat = TTCA_gma_hat;
    g_pDlg->TTCA_f_des_dot = TTCA_f_des_dot;
    g_pDlg->F_old = F_old;
    g_pDlg->F_old_time = F_old_time;
    g_pDlg->vz_target = vz_target;
    g_pDlg->var_freq_force = var_freq_force;

    return 0;
}
