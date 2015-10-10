using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Threading;

using NavMesh;

namespace NavMeshTest {
    public partial class MainWindow : Window {
        private bool CheckFilename {
            get { return File.Exists(Filename.Text); }
        }
        private bool CheckXStart {
            get {
                float value;
                return Single.TryParse(XStart.Text, out value);
            }
        }
        private bool CheckYStart {
            get {
                float value;
                return Single.TryParse(YStart.Text, out value);
            }
        }
        private bool CheckZStart {
            get {
                float value;
                return Single.TryParse(ZStart.Text, out value);
            }
        }
        private bool CheckXEnd {
            get {
                float value;
                return Single.TryParse(XEnd.Text, out value);
            }
        }
        private bool CheckYEnd {
            get {
                float value;
                return Single.TryParse(YEnd.Text, out value);
            }
        }
        private bool CheckZEnd {
            get {
                float value;
                return Single.TryParse(ZEnd.Text, out value);
            }
        }

        private IList<Point> GetPoints(string filename, NavMesh.Vector start, NavMesh.Vector end) {
            return Mesh.FindPath(filename, start, end).Select(vector => new Point(vector.X, vector.Y)).ToList();
        }

        public MainWindow() {
            InitializeComponent();

            var timer = new DispatcherTimer();
            timer.Tick += Timer_Tick;
            timer.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timer.Start();
        }

        private void Timer_Tick(Object sender, EventArgs e) {
            ButtonFindPath.IsEnabled =
                CheckFilename
                && CheckXStart
                && CheckYStart
                && CheckZStart
                && CheckXEnd
                && CheckYEnd
                && CheckZEnd;
        }

        private void ButtonFindPath_Click(System.Object sender, RoutedEventArgs e) {
            var points = GetPoints(
                Filename.Text,
                new NavMesh.Vector(
                    Single.Parse(XStart.Text),
                    Single.Parse(YStart.Text),
                    Single.Parse(ZStart.Text)
                    ),
                new NavMesh.Vector(
                    Single.Parse(XEnd.Text),
                    Single.Parse(YEnd.Text),
                    Single.Parse(ZEnd.Text)
                    )
                );

            if (!points.Any()) {
                Result.Content = "No path found!";
                Result.Background = Brushes.Salmon;
                return;
            }

            var line = new Polyline {
                Stroke = Brushes.Crimson,
                StrokeThickness = 4,
                Points = new PointCollection(points),
            };
        }

        private void Filename_TextChanged(System.Object sender, System.Windows.Controls.TextChangedEventArgs e) {
            Filename.Background = CheckFilename ? Brushes.YellowGreen : Brushes.Salmon;
        }

        private void XStart_TextChanged(System.Object sender, System.Windows.Controls.TextChangedEventArgs e) {
            XStart.Background = CheckXStart ? Brushes.YellowGreen : Brushes.Salmon;
        }
        private void YStart_TextChanged(System.Object sender, System.Windows.Controls.TextChangedEventArgs e) {
            YStart.Background = CheckYStart ? Brushes.YellowGreen : Brushes.Salmon;
        }
        private void ZStart_TextChanged(System.Object sender, System.Windows.Controls.TextChangedEventArgs e) {
            ZStart.Background = CheckZStart ? Brushes.YellowGreen : Brushes.Salmon;
        }
        private void XEnd_TextChanged(System.Object sender, System.Windows.Controls.TextChangedEventArgs e) {
            XEnd.Background = CheckXEnd ? Brushes.YellowGreen : Brushes.Salmon;
        }
        private void YEnd_TextChanged(System.Object sender, System.Windows.Controls.TextChangedEventArgs e) {
            YEnd.Background = CheckYEnd ? Brushes.YellowGreen : Brushes.Salmon;
        }
        private void ZEnd_TextChanged(System.Object sender, System.Windows.Controls.TextChangedEventArgs e) {
            ZEnd.Background = CheckZEnd ? Brushes.YellowGreen : Brushes.Salmon;
        }
    }
}
