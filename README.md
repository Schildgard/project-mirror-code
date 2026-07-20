# Project Mirror

> An atmospheric third-person **action-adventure / soulslike-horror** built from the ground up in **Unreal Engine 5 (C++)**.
> Working pitch: *"Silent Hill in the clothes of Demon's Souls."*

**Project Mirror** (narrative working title *Glass Purgatory*) is the debut title of a two-person indie team. This repository contains the game's **C++ gameplay framework** — the systems, architecture and tooling that the game is built on.

I share it publicly as an **engineering portfolio piece**: the focus below is on *how the code is structured*, not on selling a finished game.

> This repository is a **source-and-documentation extract** of the full project — it contains `Source/` and `Docs/` only, without engine content or art assets, and is therefore not buildable on its own.

> **Status — early development.** This is a vertical-slice / prototype build. Several systems are intentionally still open (see [`Docs/OffenePunkte.txt`](Docs/OffenePunkte.txt)). The value on display here is the **architecture and the discipline around it**, not feature-completeness.

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

### Mirror puzzle: proximity-driven events + material blending
The first concrete mechanic built on the event system — a staircase that only becomes real while the player looks into a mirror from the right distance:

- `AInteractableMysteriousMirror` is a **non-input-driven** interactable: it fires events from **proximity + view-angle** (nested sphere/box trigger volumes plus a per-frame facing dot-product), not from the interaction button.
- Events are deliberately **transient** — fired straight through `USubsystemObjectEvents` *without* touching the persisted `ActivatedObjects` list, so the effect is live-only and never saved.
- `ATriggerableIllusionaryObject` reacts with a **smooth material blend** instead of an instant toggle: a dynamic material instance whose alpha is `FInterpTo`-driven, flipping visibility/collision at the blend endpoints. The same class also backs **illusory walls** that only dissolve once struck — implementing `IDamageable`, they ride the ordinary combat hit chain with no special case on the attacking side.
- The mirror also renders a true **planar real-time reflection** via a `USceneCaptureComponent2D`: the player camera is reflected across the mirror plane (position + orientation), a clip plane culls geometry behind the glass, and the capture is gated on `WasRecentlyRendered` so off-screen mirrors cost nothing.

→ Full write-up: [`Docs/MirrorStaircaseSystem.txt`](Docs/MirrorStaircaseSystem.txt)

### Save/Load & level-transition lifecycle
Dedicated `USubsystemSaveAndLoad` and `USubsystemLevelLoading` services driven by command delegates (`FCommandSaveGame` / `FCommandLoadGame`) instead of logic baked into actors.

- **Two deliberate paths for player state:** an in-memory cache *with* temporary buffs survives a level transition, while a save-game load restores *base stats only* — buffs are gone by design.
- **The save schema encodes that split:** `FPlayerDataDiskAndMemory` (attributes, health, loadout) is what travels between levels; `FPlayerDataDiskOnly` (transform) is written but never cached.
- **Preload before `OpenLevel`** keeps a `Memory == Disk` invariant so `BeginPlay` is always correct.
- **Three kinds of world state** are tracked separately: one-shot activations, dropped items with their transforms, and continuous float states (e.g. how much a healing spring has left) — with the first two held as mutually exclusive by invariant, so an item is either picked up or lying in the world, never both.

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

### Data-driven equipment & weapon system
Characters carry a `UEquipmentComponent` that resolves *where* an item goes and renders it on the skeleton — all driven by `UDefinitionEquipment` data assets (slot, visual type, weapon type, sockets, combat stats), not hardcoded:

- **Smart slot resolution** (`ResolveEquipmentSlot`) yields **dual-wielding** (a second one-hander moves to the off-hand), **two-handing** (clears both hands), and **three distinct backup stow slots** (belt / back / shield-back) when the hands are full — the caller never has to know, but gets the resolved slot back.
- **Polymorphic attach:** an `AEquipmentBase` hierarchy (`AEquipmentStatic` / `AEquipmentSkeletal` / `AWeaponStaticBase`) where `AttachToOwner` handles socket attachment vs. leader-pose skinning behind one virtual call.
- **Frame-accurate pickup choreography:** a proxy object in the hand plus the real item hidden at its destination, committed by *two* separate animation notifies — so an item is never visible in two places during a stow.
- **Weapon effects as components** (`UEffectComponentBase`) — a burning torch is an independently toggleable component whose state survives level transitions and save/load.
- **Animation hookup** via an `OnWeaponChanged` delegate (per-hand `EWeaponType` drives anim layers), including a **mirrored "take" montage** when the item will land in the off-hand.
- **Persistent** through `FSavedEquipmentData` (asset references and state flags only — the save stays lean), including items dropped on the ground.

→ Full write-up: [`Docs/EquipmentSystem.txt`](Docs/EquipmentSystem.txt)

### Combat, attributes and the hit chain
A **purpose-built attribute and combat layer** rather than the Gameplay Ability System — the game is single-player, so GAS would have added replication machinery and debugging cost without a matching payoff.

- **Three-layer attribute model** (`UAttributeComponent`): base values → effective (gear/buffs) → derived pools. Loading uses a *separate* code path from levelling so that restoring a save cannot silently heal the player.
- **Pools own their consumers:** `UStaminaComponent` holds the exhaustion state itself instead of each consumer (sprint, attack, block) re-implementing it; `URunComponent` is composed onto the controller.
- **No combat-state enum by design** — attacking and holding a guard must be able to overlap, so the state is *derived* from two facts (`IsPerformingAttack()`, `HasActiveSustainedAction()`) instead of stored in mutually exclusive values.
- **Combos via montage sections**, redirected seamlessly with `Montage_SetNextSection` and gated by an `AnimNotifyState` window.
- **Capsule-sweep hit detection** (not overlaps — overlaps carry no physical material, which the surface-aware impact feedback needs), with per-swing target de-duplication.
- **`IDamageable` puts mitigation on the receiver:** the attacker states only what the hit consists of, never how much it hurts. A destructible wall and an enemy use the same path with no special-casing.
- **Two-stage feedback resolution:** who was hit (creature channel set vs. world physical material) and then which damage channel dominates — only the dominant channel spawns an effect, so a burning sword throws sparks of one kind, not two.

→ Full write-up: [`Docs/CombatSystem.txt`](Docs/CombatSystem.txt)

### Ledge climbing
`UClimbComponent` detects grabbable ledges with two chained sphere traces — one for "is there a wall", one probing *inward and downward* for a genuinely walkable top surface — then hangs, shimmies (planned) and pulls up. Movement state, attachment and input gating are driven by an explicit `EClimbingState` enum so that "hanging" and "pulling up" cannot be confused; every entry point guards its expected source state, and every animation path has a direct fallback so a failed montage can never strand the character.

→ Full write-up: [`Docs/ClimbSystem.txt`](Docs/ClimbSystem.txt)

### Lock-on / focus targeting
`ULockOnComponent` sphere-traces for actors implementing `IFocusable`, locks the camera/character with smooth `RInterpTo` rotation, and supports left/right target switching via local-space relative positioning.

### Surface-aware footstep audio
`UFootstepAudioComponent` line-traces under each foot, reads the **physical-material surface type**, and swaps the cached footstep sound accordingly — driven by animation notifies and configurable per **creature type** through `USettingsSound`.

→ Full write-up: [`Docs/FootstepSystem.txt`](Docs/FootstepSystem.txt)

### CommonUI + MVVM front-end
HUD and menus built on `UCommonActivatableWidgetStack` layers with explicit input-mode resolution (gameplay vs. menu), backed by **UE MVVM ViewModels** for clean data binding between logic and UMG.

A generic `UVMProgressbar` (current / max / percent) backs both health and stamina through the same view model. Bindings are established in **C++ via `FieldNotify`** rather than in the editor's MVVM panel — editor-authored bindings did not survive asset reimports, and moving them into code made the data flow both durable and reviewable in version control.

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
| [`MirrorStaircaseSystem.txt`](Docs/MirrorStaircaseSystem.txt) | Proximity/facing-driven mirror events, material blending, planar reflection, illusory walls |
| [`EquipmentSystem.txt`](Docs/EquipmentSystem.txt) | Data-driven equipment/weapons, slot resolution, pickups, save persistence |
| [`CombatSystem.txt`](Docs/CombatSystem.txt) | Attributes and pools, action modes, combos, hit chain, impact feedback, anim-graph layering |
| [`ClimbSystem.txt`](Docs/ClimbSystem.txt) | Ledge detection, hanging, pull-up, state guards |
| [`BgmSystem.txt`](Docs/BgmSystem.txt) | Stack-based music/ambient, async loading, crossfades, area volumes |
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

This project is developed with **AI-assisted tooling (Claude Code)** under my own documented conventions. Architecture, system design and all engineering decisions are mine; the AI is used as a controlled accelerator (boilerplate, refactoring, documentation). Prior solo projects — built long before these tools existed — demonstrate the underlying engineering is my own.

---

## License

© 2026 Leonard Kemenani. All rights reserved.
Source is published for **portfolio and review purposes**. Not licensed for redistribution or reuse without permission.