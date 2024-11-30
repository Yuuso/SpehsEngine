#pragma once


namespace se
{
    enum class NoiseType
    {
        /**
         * 2D Simplex noise, standard lattice orientation.
         */
        OpenSimplex_2D_Default,

        /**
         * 2D Simplex noise, with Y pointing down the main diagonal.
         * Might be better for a 2D sandbox style game, where Y is vertical.
         * Probably slightly less optimal for heightmaps or continent maps,
         * unless your map is centered around an equator. It's a subtle
         * difference, but the option is here to make it an easy choice.
         */
        OpenSimplex_2D_ImproveX,

        /**
         * 2D Simplex noise base.
         */
        OpenSimplex_2D_UnskewedBase,

        /**
         * 3D OpenSimplex2 noise, with better visual isotropy in (X, Y).
         * Recommended for 3D terrain and time-varied animations.
         * The Z coordinate should always be the "different" coordinate in whatever your use case is.
         * If Y is vertical in world coordinates, call Noise3_ImproveXZ(x, z, Y) or use noise3_XZBeforeY.
         * If Z is vertical in world coordinates, call Noise3_ImproveXZ(x, y, Z).
         * For a time varied animation, call Noise3_ImproveXY(x, y, T).
         */
        OpenSimplex_3D_ImproveXY,

        /**
         * 3D OpenSimplex2 noise, with better visual isotropy in (X, Z).
         * Recommended for 3D terrain and time-varied animations.
         * The Y coordinate should always be the "different" coordinate in whatever your use case is.
         * If Y is vertical in world coordinates, call Noise3_ImproveXZ(x, Y, z).
         * If Z is vertical in world coordinates, call Noise3_ImproveXZ(x, Z, y) or use Noise3_ImproveXY.
         * For a time varied animation, call Noise3_ImproveXZ(x, T, y) or use Noise3_ImproveXY.
         */
        OpenSimplex_3D_ImproveXZ,

        /**
         * 3D OpenSimplex2 noise, fallback rotation option
         * Use Noise3_ImproveXY or Noise3_ImproveXZ instead, wherever appropriate.
         * They have less diagonal bias. This function's best use is as a fallback.
         */
        OpenSimplex_3D_Fallback,

        /**
         * Generate overlapping cubic lattices for 3D OpenSimplex2 noise.
         */
        OpenSimplex_3D_UnrotatedBase,

        /**
         * 4D OpenSimplex2 noise, with XYZ oriented like Noise3_ImproveXY
         * and W for an extra degree of freedom. W repeats eventually.
         * Recommended for time-varied animations which texture a 3D object (W=time)
         * in a space where Z is vertical
         */
        OpenSimplex_4D_ImproveXYZ_ImproveXY,

        /**
         * 4D OpenSimplex2 noise, with XYZ oriented like Noise3_ImproveXZ
         * and W for an extra degree of freedom. W repeats eventually.
         * Recommended for time-varied animations which texture a 3D object (W=time)
         * in a space where Y is vertical
         */
        OpenSimplex_4D_ImproveXYZ_ImproveXZ,

        /**
         * 4D OpenSimplex2 noise, with XYZ oriented like Noise3_Fallback
         * and W for an extra degree of freedom. W repeats eventually.
         * Recommended for time-varied animations which texture a 3D object (W=time)
         * where there isn't a clear distinction between horizontal and vertical
         */
        OpenSimplex_4D_ImproveXYZ,

        /**
         * 4D OpenSimplex2 noise, fallback lattice orientation.
         */
        OpenSimplex_4D_Fallback,

        /**
        * 4D OpenSimplex2 noise base.
        */
        OpenSimplex_4D_UnskewedBase,

        Size,
    };

    class NoiseGenerator
    {
    public:

        NoiseGenerator(int64_t _seed = 1);
        void setSeed(int64_t _seed);

        float generate(double _x, double _y, NoiseType _type) const;
        float generate(double _x, double _y, double _z, NoiseType _type) const;
        float generate(double _x, double _y, double _z, double _w, NoiseType _type) const;

    private:

        int64_t seed;
    };
}
