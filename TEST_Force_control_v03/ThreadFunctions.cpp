#include "pch.h"
#include "ThreadFunctions.h"
#include "TEST_Force_control_v03Dlg.h"
#include "UtilityFunctions.h"
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

UINT Thread_polishing(LPVOID pParam)
{
    CTESTForcecontrolv03Dlg* g_pDlg = (CTESTForcecontrolv03Dlg*)pParam;

    // �ֱ� ���� �� ������ ���� ����
    system_clock::time_point ts;
    nanoseconds t_duration_ns;
    double t_duration_ns_double = 0.0;
    int i_freq = 0;
    double td_s_sum = 0.0;

    // �ʱ� ���������� ��ġ ��ǥ
    float pos[3] = { g_pDlg->ini_pos_actual[0], g_pDlg->ini_pos_actual[1], g_pDlg->ini_pos_actual[2] };

    while (g_pDlg->m_bRunThread_polishing)
    {        
        ts = system_clock::now();

        // �ʱ� ��ġ ���� Edit ��Ʈ�ѿ� ����
        SetEditText(g_pDlg->var_posX, pos[0]);
        SetEditText(g_pDlg->var_posY, pos[1]);
        SetEditText(g_pDlg->var_posZ, pos[2]);

        pos[0] += 1.0;
        pos[2] += 2.0;

        //////////////////////////////////////////////////////////////////////////
        ////while�� �ӵ� ����+++++++++++++++++++++++++++++++++++++++++++++++++++++
        //////////////////////////////////////////////////////////////////////////

        t_duration_ns_double = 0.0;
        while (t_duration_ns_double < 1000000) // 1 millisecond
        {
            t_duration_ns = system_clock::now() - ts;
            t_duration_ns_double = double(t_duration_ns.count());
        }
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        ////�ֱ� ���� �� ���+++++++++++++++++++++++++++++++++++++++++++++++++++++
        //////////////////////////////////////////////////////////////////////////
        i_freq++;
        td_s_sum += (t_duration_ns_double / 1000000000.0);

        if (i_freq == 1000)
        {
            g_pDlg->var_freq_force = 1.0 / (td_s_sum / 1000.0);
            CString strFreq;
            strFreq.Format(_T("%.2f Hz"), g_pDlg->var_freq_force);
            g_pDlg->m_var_freq_force.SetWindowTextW(strFreq);

            i_freq = 0;
            td_s_sum = 0.0;
        }
        //////////////////////////////////////////////////////////////////////////
    }

    return 0;

}


UINT Thread_force_test(LPVOID pParam)
{
    CTESTForcecontrolv03Dlg* g_pDlg = (CTESTForcecontrolv03Dlg*)pParam;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(8, 12); // 10 �� 2�� ���� ����

    // �ֱ� ���� �� ������ ���� ����
    high_resolution_clock::time_point ts, t_start;
    nanoseconds t_duration_ns;
    double t_duration_ns_double = 0.0;
    int i_freq = 0;
    double td_s_sum = 0.0;

    // �ð� ������ ������ ���� ����
    nanoseconds t_stamp_ns;
    float t_stamp_ns_float = 0.0;

    // �ʱ� ���������� ��ġ ��ǥ
    float pos[3] = { g_pDlg->ini_pos_actual[0], g_pDlg->ini_pos_actual[1], g_pDlg->ini_pos_actual[2] };

    t_start = high_resolution_clock::now();

    const int update_period_ms = 1000 / 16; // 16Hz ������Ʈ �ֱ�
    int update_counter = 0;

    while (g_pDlg->m_bRunThread_force)
    {
        // �ֱ� ���� �ð� ���
        ts = high_resolution_clock::now();

        // 10 �� 2�� ���� �����ϰ� Edit box�� ����
        int rand_force = dis(gen);
        CString str_randforce;
        str_randforce.Format(_T("%d"), rand_force);
        g_pDlg->m_var_force.SetWindowTextW(str_randforce);

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
            data->rand_force = rand_force;
            data->freq = g_pDlg->var_freq_force;
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
            g_pDlg->var_freq_force = 1.0 / (td_s_sum / 1000.0);
            CString strFreq;
            strFreq.Format(_T("%.2f Hz"), g_pDlg->var_freq_force);
            g_pDlg->m_var_freq_force.SetWindowTextW(strFreq);

            i_freq = 0;
            td_s_sum = 0.0;
        }
        //////////////////////////////////////////////////////////////////////////
    }

    return 0;
}
