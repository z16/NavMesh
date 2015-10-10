using System;
using System.Runtime.InteropServices;

namespace NavMesh {
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector {
        [MarshalAs(UnmanagedType.R4)]
        public Single X;
        [MarshalAs(UnmanagedType.R4)]
        public Single Y;
        [MarshalAs(UnmanagedType.R4)]
        public Single Z;

        public Vector(Single x, Single y, Single z) {
            X = x;
            Y = y;
            Z = z;
        }
    }
}
