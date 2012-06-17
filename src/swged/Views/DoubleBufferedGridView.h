
#pragma once

#using <System.Windows.Forms.dll>

ref class DoubleBufferedDataGridView : public System::Windows::Forms::DataGridView
{
public:
    DoubleBufferedDataGridView()
        : System::Windows::Forms::DataGridView()
    {
        DoubleBuffered = true;
    }
};
