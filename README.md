# Irregular Sliding Puzzle

[中文版本](./README_zh.md)

A sliding puzzle is a classic logic game where players rearrange shuffled tiles into the correct order by sliding them into empty spaces.

**Irregular Sliding Puzzle** builds on this timeless challenge with a unique twist: fully customizable board shapes. Break free from traditional rectangular grids and experiment with diverse, user-defined layouts that heighten both challenge and variety. Perfect for puzzle enthusiasts seeking a fresh and innovative experience.

## Installation

The Microsoft Store is now the exclusive and official platform for downloading this game. You can get it by visiting the [Microsoft Store web page](https://apps.microsoft.com/detail/9NCJ4RNQXC7R) (GitHub does not support direct links to the Microsoft Store app).

Of course, you can also install it via `winget` by executing the command `winget install 9NCJ4RNQXC7R`.

## Design

When you launch the game, you’ll first enter the **board design page**. Use the icons on the left and top side to add or remove rows or columns. There are three editing modes: **Add**, **Reverse** (default), and **Erase**.

### Add

![](Irregular%20Sliding%20Puzzle/Assets/add.gif)

### Reverse

![](Irregular%20Sliding%20Puzzle/Assets/reverse.gif)

### Erase

![](Irregular%20Sliding%20Puzzle/Assets/erase.gif)

## Play!

After designing your custom map, click the **Start** icon on the top bar to begin. If the map is valid, the game board will appear:

- On the **left**: the target arrangement and the tool bar
- On the **right**: the puzzle board for you to solve

Click any tile in the same row or column as the empty cell to slide it. The tiles between will shift toward the empty space.

You can **pause or resume** the game at any time. If your puzzle is too hard to solve, click the **Surrender** icon to return to the design page.

## Game Record

Your whole playing process is automatically recorded and saved in the folder `%LOCALAPPDATA%\Packages\PureHappiness.9278D8CD1E3F_3k498pdbhk64a\LocalState`, and you can review it by clicking the corresponding **replay** icon on the **history** board.