using OpenHardwareMonitor.Hardware;
using SurfanceProControl.TemperatureReder;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SurfanceProControl
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        //VARS
//CIAO
        bool AutoMode = false;
        bool ActiveFanCtrl = false;
        bool EnableLog = false;
        string SelectedCOM;
        public List<string> COMPorts = new List<string>();
        List<string> LogOutput = new List<string>();

        public MainWindow()
        {
            
            
            InitializeComponent();
            LoadCOMPorts();
            CpuTemperatureReader tempReader = new CpuTemperatureReader();
            var temps = tempReader.GetTemperaturesInCelsius();
        }

        private void LoadCOMPorts()
        {
            COMPorts = SerialPort.GetPortNames().ToList();
            COMPortsCombo.ItemsSource = COMPorts;
            if (EnableLog)
                LogOutput.Add("COM Loaded");
        }

        private void StartFanCtrl_Btn(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }
        private void StopFanCtrl_Btn(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }
       
        private void SetFanSpeed_Slider(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }

        private void LogEnabling_IsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            throw new NotImplementedException();

        }


        private void AutoMode_IsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            throw new NotImplementedException();

        }
        private void ManualMode_IsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            throw new NotImplementedException();

        }

        private void COMPort_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SelectedCOM = sender.ToString();
            if (EnableLog)
                LogOutput.Add("Selected COM: " + SelectedCOM);

        }

        private void RadioButton_IsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {

        }

        private void LogEnabling_Checked(object sender, RoutedEventArgs e)
        {
            EnableLog = !EnableLog;
        }

       

        
    }
}
