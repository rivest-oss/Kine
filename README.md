# Kine

What you will see when you first open the program are two things:

* Little green-to-red circles, that are **worms**.
* The map behind them, which is represented in grayscale,
  with **black** being **absence of food**, and **white** as
  **plenty of food**.

There, you will see that even with an extremely simple algorithm,
is actually capable of "preferring" an environment with lots of
food, and "hating" environment with little to no food.

Pressing Function keys will cause the map to regenerate as:

* **F1**: split into half.
* **F2**: smooth Perlin noise.
* **F3**: vertical gradient.
* **F4**: sharp circle.
* **F5**: Voronoi/Cellular noise.
* other functions keys: same as **F1**.

Also,

* with the `S` key, you can make the whole simulation faster
  (up to 1000x).
* with the `A` key, you can make the whole simulation slower
  (at min. 1x).
* with the `I`, you can invert the amount of food in all cells.
* you can drag & drop an image (PNG, JPG and GIF) to use it as
  food cell.

**Inspired by:**

* [Kinesis, a real thing in biology](https://en.wikipedia.org/wiki/Kinesis_(biology)).

---

Yes, I know the code sucks.
