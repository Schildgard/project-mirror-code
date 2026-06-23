# Project Mirror

> An atmospheric third-person **action-adventure / soulslike-horror** built from the ground up in **Unreal Engine 5 (C++)**.
> Working pitch: *"Silent Hill in the clothes of Demon's Souls."*

**Project Mirror** (narrative working title *Glass Purgatory*) is the debut title of a two-person indie team. This repository contains the game's **C++ gameplay framework** — the systems, architecture and tooling that the game is built on.

I share it publicly as an **engineering portfolio piece**: the focus below is on *how the code is structured*, not on selling a finished game.

> **Status — early development.** This is a vertical-slice / prototype build. Several systems are intentionally still open. The value on display here is the **architecture and the discipline around it**, not feature-completeness.

---

## My role

This is a **two-person project**: my partner handles all art and assets, and I am the **sole programmer and co-founder**, responsible for the **entire software architecture** and the **majority of the game design**.

Everything in `Source/` is my own work. The codebase is deliberately built as a **reusable framework** intended to outlive this project and seed the next one.

---

## Tech stack

| Area | Details |
|---|---|
| Engine | **Unreal Engine 5.7** |
| Language | **Modern C++** — `TObjectPtr` / `TSoftObjectPtr`, `TWeakObjectPtr`, delegates, `FName`/`FGuid` identities |
| Architecture | `UGameInstanceSubsystem` services, `UActorComponent` composition, C++ interfaces |
| UI | **CommonUI** (activatable widget stacks) + **UE MVVM** (`ModelViewViewModel` plugin) |
| Config | `UDeveloperSettings` classes (no hardcoded gameplay values where avoidable) |
| Audio | Async-streamed music/ambient stack, surface-aware footstep system |
| Tooling | Perforce / Git, in-repo system documentation under [`Docs/`](Docs/) |

---

## Engineering highlights

The systems below are the reason this repo is worth reading.

### Decoupled World-Event system (Pub/Sub via a mediator subsystem)
Interactable objects fire an `EventID`; triggerable objects react — without ever knowing about each other. Communication is brokered by `USubsystemObjectEvents`:

- **Same-level** signalling through a `TMap<FName, FSimpleMulticastDelegate>`.
- **Cross-level persistence** through a saved `ActivatedObjects` list, preloaded *before* `OpenLevel` so objects already see the correct state in `BeginPlay`.
- Per-reaction configuration (`FEventReactionConfig`): restore-from-history vs. live reaction, independently toggleable.
- **`WeakLambda` bindings** for memory safety, with documented idempotency edge cases.

→ Full write-up: [`Docs/WorldEventSystem.txt`](Docs/WorldEventSystem.txt)

### Save/Load & level-transition lifecycle
Dedicated `USubsystemSaveAndLoad` and `USubsystemLevelLoading` services driven by command delegates (`FCommandSaveGame` / `FCommandLoadGame`) instead of logic baked into actors.

- **Two deliberate paths for player state:** an in-memory cache *with* temporary buffs survives a level transition, while a save-game load restores *base stats only* — buffs are gone by design.
- **Preload before `OpenLevel`** keeps a `Memory == Disk` invariant so `BeginPlay` is always correct.

### Subsystem-based service architecture
Clear separation of concerns across `UGameInstanceSubsystem`s: **Save/Load**, **Level Loading**, **Object Events**, and **BGM** — responsibilities live in services, not in actors.

### Stack-based BGM / ambient system
`USubsystemBgm` manages music and ambient sound as two independent stacks of `UAudioComponent`s:

- **Asynchronous** sound loading via `TSoftObjectPtr` + the streamable manager.
- **Crossfades** with push/pause/resume semantics — e.g. opening the pause menu ducks ambient and layers menu music, then restores the previous track on close.
- Level-driven track selection from `USettingsSound` maps, with PIE-prefix normalization and `TWeakObjectPtr<UWorld>` guards against stale async callbacks across level loads.

### Animation-driven interaction system
Interaction is **frame-accurate**, not instantaneous: `UInteractionComponent` tracks interactables in range and resolves the nearest valid `CurrentInteractable`. Triggering plays a montage, snaps the character to the target, and an **`AnimNotify_Interact`** fires the actual interaction at the correct animation frame — driven through the `IInteractable` interface.

→ Full write-up: [`Docs/InteractionSystem.txt`](Docs/InteractionSystem.txt)

### Lock-on / focus targeting
`ULockOnComponent` sphere-traces for actors implementing `IFocusable`, locks the camera/character with smooth `RInterpTo` rotation, and supports left/right target switching via local-space relative positioning.

### Surface-aware footstep audio
`UFootstepAudioComponent` line-traces under each foot, reads the **physical-material surface type**, and swaps the cached footstep sound accordingly — driven by animation notifies and configurable per **creature type** through `USettingsSound`.

→ Full write-up: [`Docs/FootstepSystem.txt`](Docs/FootstepSystem.txt)

### CommonUI + MVVM front-end
HUD and menus built on `UCommonActivatableWidgetStack` layers with explicit input-mode resolution (gameplay vs. menu), backed by **UE MVVM ViewModels** for clean data binding between logic and UMG.

→ Full write-up: [`Docs/IngameMenu_HUD.txt`](Docs/IngameMenu_HUD.txt)

### Interface-driven, config-driven design
Small, focused C++ interfaces (`ISaveable`, `IInteractable`, `IFocusable`, `ITriggerable`) and `UDeveloperSettings` classes for character, sound, save and UI configuration keep systems decoupled and data-driven. Player camera, lock-on and interaction components are composed onto the pawn at runtime in `OnPossess`.

---

## Documentation

This project keeps **living technical documentation** in [`Docs/`](Docs/) — a deliberate habit, not an afterthought:

| Doc | Covers |
|---|---|
| [`WorldEventSystem.txt`](Docs/WorldEventSystem.txt) | Pub/Sub event system, persistence, flows, known limitations |
| [`InteractionSystem.txt`](Docs/InteractionSystem.txt) | Interaction component, animation-notify chain, lock-on/focus |
| [`BgmSystem.txt`](Docs/BgmSystem.txt) | Stack-based music/ambient, async loading, crossfades |
| [`FootstepSystem.txt`](Docs/FootstepSystem.txt) | Surface-aware footstep audio |
| [`IngameMenu_HUD.txt`](Docs/IngameMenu_HUD.txt) | CommonUI HUD/menu layering and input modes |
| [`OffenePunkte.txt`](Docs/OffenePunkte.txt) | Tracked tech debt and open points (with `file:line` references) |
| [`framework_todo.txt`](Docs/framework_todo.txt) | Planned framework/reuse decoupling steps |

---

## Media

<!-- Add gameplay screenshots / GIFs here, e.g.:
![Mirror room](Docs/media/mirror_room.png)
-->

*Screenshots and gameplay clips to be added.*

---

## A note on workflow

This project is developed with **AI-assisted tooling (Claude Code)** under my own documented conventions. Architecture, system design and all engineering decisions are mine. the AI is used as a controlled accelerator ( reviewing, refactoring, documentation).

---

## License

© 2026 Leonard Kemenani. All rights reserved.
Source is published for **portfolio and review purposes**. Not licensed for redistribution or reuse without permission.
