#ifndef COORD_H
#define COORD_H

namespace aw
{
	class coord
	{
		public:
			coord(int x = -1, int y = -1)
			: x(x), y(y)
			{
			}

			inline void reset()
			{
				this->x = -1;
				this->y = -1;
			}

			inline void assign(int x, int y)
			{
				this->x = x;
				this->y = y;
			}

			inline operator bool() const
			{
				return (x != -1 && y != -1);
			}

			 int x;
			 int y;
	};

	inline bool operator==(const coord &a, const coord &b)
	{
		return (a.x == b.x && a.y == b.y);
	}

	inline bool operator!=(const coord &a, const coord &b)
	{
		return !(a == b);
	}

	inline bool operator<(const coord &a, const coord &b)
	{
		return (a.x < b.x || (b.x >= a.x && a.y < b.y)); //0|1 -> 0|2 -> 1|0 -> 1|1 //X first, then Y
//		return (a.y < b.y || (b.y >= a.y && a.x < b.x));  //0|0 ->1|0 -> 2|0 -> 0|1 -> 0|2 //Y first, then X
	}
}

#endif // COORD_H
