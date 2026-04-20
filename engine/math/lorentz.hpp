#ifndef __LORENTZ_HPP
#define __LORENTZ_HPP

// Header fajl koja implementira Lorentzove transformacije
#include <cmath>
#include <glm/glm.hpp>


/**
 * 
 */


namespace Engine
{
    namespace Lorentz
    {
        inline float minkowskiDot(const glm::vec4& a, const glm::vec4& b) {
            return a.x * b.x + a.y * b.y + a.z * b.z - a.w * b.w;
        }
        inline float gamma(float v) {
            return 1.0f / sqrt(1.0f - v * v); // rapidnost u jedinicama brzine svijetlost
        }
        /**
         * Sve translacije u hiperbolickom prostoru se mogu predstaviti putem pokreta koji se nazivaju Lorentzove boost transformacije.
         * Ove transformacije su analogne rotacijama u euklidskom prostoru, ali se dešavaju u Minkowski prostoru koji je model za prostor-vreme u specijalnoj relativnosti.
         * Boost transformacije se mogu predstaviti kao matrice koje zavise od brzine objekta i pravca kretanja. 
         * Na primer, boost transformacija duž x ose sa brzinom v može se predstaviti kao:
         * | -γβ  γ   0   0 |
         * | 0    0   1   0 |
         * | 0    0   0   1 |
         * | γ   -γβ  0   0 |
         * Gde je β = v/c i γ = 1/sqrt(1-β^2). Ova matrica transformiše četvorovektor (x, y, z, t) u novi četvorovektor koji odgovara posmatraču koji se kreće brzinom v duž x ose.
         */
        // Boost duz x ose
        inline glm::mat4 lorentzBoostX(float v) {
            float g = 1.0f / sqrt(1.0f - v * v);

            glm::mat4 m(1.0f);

            m[0][0] =  g;      
            m[0][3] = -g * v;    
            m[3][0] = -g * v;    
            m[3][3] =  g;        

            return m;
        }
        // Boost duz y ose
        inline glm::mat4 lorentzBoostY(float v) {
            float g = gamma(v);

            glm::mat4 m(1.0f); 

            m[1][1] =  g;
            m[1][3] = -g * v;
            m[3][1] = -g * v;
            m[3][3] =  g;

            return m;
        }
        // Boost duz z ose
        inline glm::mat4 lorentzBoostZ(float v) {
            float g = gamma(v);

            glm::mat4 m(1.0f);

            m[2][2] =  g;
            m[2][3] = -g * v;
            m[3][2] = -g * v;
            m[3][3] =  g;

            return m;
        }

        /**
         * Numericka greska se akumulira zbog nepreciznosti u reprezentaciji brojeva sa pokretnim zarezom, posebno kada se radi sa velikim brzinama, blizu brzine svijetlost.
         * Zbog toga, vrsimo normalizaciju
         */
        inline void normalize(glm::mat4& m) {
            glm::vec4 x = m[0]; // Desno (Right)
            glm::vec4 y = m[1]; // Gore (Up)
            glm::vec4 z = m[2]; // Napred (Forward)
            glm::vec4 w = m[3]; // Pozicija kamere (Time-like vektor)

            // Korak 1: Normalizacija vremenske pozicije (W) na normu Minkovskog od -1
            w = w / std::sqrt(-minkowskiDot(w, w));

            // Korak 2: Ortogonalizacija i normalizacija Z vektora u odnosu na W
            // Koristimo '+' prilikom oduzimanja projekcije jer je norma W vektora negativna (-1)
            z = z + minkowskiDot(z, w) * w; 
            z = z / std::sqrt(minkowskiDot(z, z));

            // Korak 3: Ortogonalizacija i normalizacija Y vektora u odnosu na W i Z
            y = y + minkowskiDot(y, w) * w;
            y = y - minkowskiDot(y, z) * z;
            y = y / std::sqrt(minkowskiDot(y, y));

            // Korak 4: Ortogonalizacija i normalizacija X vektora u odnosu na W, Z i Y
            x = x + minkowskiDot(x, w) * w;
            x = x - minkowskiDot(x, z) * z;
            x = x - minkowskiDot(x, y) * y;
            x = x / std::sqrt(minkowskiDot(x, x));

            // Vraćanje korigovanih i ortogonalnih kolona nazad u matricu
            m[0] = x; 
            m[1] = y; 
            m[2] = z; 
            m[3] = w;
        }

        /**
         * Inverzna matrica Lorentzovog boosta se 
         */
        inline glm::mat4 inverse(const glm::mat4& state) {
            glm::mat4 view = glm::transpose(state);
            view[0][3] = -view[0][3];
            view[1][3] = -view[1][3];
            view[2][3] = -view[2][3];
            view[3][0] = -view[3][0];
            view[3][1] = -view[3][1];
            view[3][2] = -view[3][2];
            return view;
        }
    }
}


#endif