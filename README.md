# Irregular Sliding Puzzle

[中文版本](./README_zh.md)

A sliding puzzle is a classic logic game where players rearrange shuffled tiles into the correct order by sliding them into empty spaces.

**Irregular Sliding Puzzle** builds on this timeless challenge with a unique twist: fully customizable map shapes. Break free from traditional rectangular grids and experiment with diverse, user-defined layouts that heighten both challenge and variety. Perfect for puzzle enthusiasts seeking a fresh and innovative experience.

## System Requirements

- Windows 11
- Windows 10, version 1809 (10.0.17763.0) or later

> There is currently no plan to support Linux, macOS, or older versions of Windows.

## Installation

1. Enable **Developer Mode** in your system settings.
2. Download the package that matches your computer’s architecture from the [release page](../../releases/latest).
3. Extract all files from the package, then run `Install.ps1` and follow the on-screen instructions.

> The game will also be published in the Microsoft Store in the future for easier installation.

## Design

When you launch the game, you’ll first enter the **map design page**. Use the icons on the left and top side to add or remove rows or columns. There are three editing modes: **Add**, **Reverse** (default), and **Erase**.

### Add

![](Irregular%20Sliding%20Puzzle/Assets/add.gif)

### Reverse

![](Irregular%20Sliding%20Puzzle/Assets/reverse.gif)

### Erase

![](Irregular%20Sliding%20Puzzle/Assets/erase.gif)

## Play!

After designing your custom map, click the **Start** icon on the top bar to begin. If the map is valid, the game board will appear:

- On the **left**: the target arrangement.
- On the **right**: the puzzle board for you to solve.

Click any tile in the same row or column as the empty cell to slide it. The tiles between will shift toward the empty space.

You can **pause or resume** the game at any time. If your puzzle is too hard to solve, click the **Surrender** icon to return to the design page.