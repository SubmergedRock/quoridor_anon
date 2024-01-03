// constants.hpp

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SIZE 15
const int WALLS = (SIZE == 9) ? 10 : static_cast<int>((SIZE * SIZE) / 4 - SIZE * 1.5 + 3.25);

#endif // CONSTANTS_H