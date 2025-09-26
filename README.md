# Record

This repository contains a Qt 5 desktop application for managing small sets of
credentials with a lightweight, semi-transparent interface. The original UI has
been preserved while the project layout has been reorganised for easier ongoing
maintenance.

## Project layout

```
LEARNING/
├── forms/                # Qt Designer `.ui` files
├── source/               # Resources referenced from the UI
├── src/
│   ├── app/              # Application entry point
│   ├── core/             # Data access and parsing logic
│   └── ui/               # Widgets that implement the animated password UI
│       └── widgets/      # Reusable helper widgets (animated labels, etc.)
└── LEARNING.pro          # qmake project definition
```

The refactor keeps the frameless, translucent visuals while making it easier to
locate related code and extend the application in future iterations.
