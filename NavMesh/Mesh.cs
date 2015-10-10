using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace NavMesh
{
    public class Mesh
    {
        [DllImport("CNavMesh.dll", EntryPoint = "_FindPath@28", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr _FindPath(
            [MarshalAs(UnmanagedType.LPStr)]
            String filename,
            [MarshalAs(UnmanagedType.R4)]
            Single startX,
            [MarshalAs(UnmanagedType.R4)]
            Single startY,
            [MarshalAs(UnmanagedType.R4)]
            Single startZ,
            [MarshalAs(UnmanagedType.R4)]
            Single endX,
            [MarshalAs(UnmanagedType.R4)]
            Single endY,
            [MarshalAs(UnmanagedType.R4)]
            Single endZ
        );

        public static IEnumerable<Vector> FindPath(string filename, Vector start, Vector end) {
            var ptr = _FindPath(filename, start.X, start.Y, start.Z, end.X, end.Y, end.Z);
            var size = Marshal.ReadInt32(ptr);

            for (var i = 0; i < size; ++i) {
                yield return Marshal.PtrToStructure<Vector>(ptr + 4 + i * 4);
            };
        }
    }
}
