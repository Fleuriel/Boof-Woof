#include <deque>
#include <functional>

// Singleton access to UndoRedoManager
#define g_UndoRedoManager UndoRedoManager::GetInstance()

class UndoRedoManager {
public:
    static UndoRedoManager& GetInstance() {
        static UndoRedoManager instance{};
        return instance;
    }

    struct Command {
        std::function<void()> doAction;
        std::function<void()> undoAction;
    };

    void ExecuteCommand(const std::function<void()>& doAction, const std::function<void()>& undoAction) {
        // Execute the action
        doAction();

        // Save both actions to the undo stack
        undoStack.push_back({ doAction, undoAction });

        // If the size exceeds the limit, remove the oldest command
        if (undoStack.size() > maxStackSize) {
            undoStack.pop_front();
        }

        // Clear redo stack when a new command is executed
        redoStack.clear();
    }

    void Undo() {
        if (!undoStack.empty()) {
            auto command = undoStack.back();
            undoStack.pop_back();

            // Execute the undo action
            command.undoAction();

            // Push the command to the redo stack
            redoStack.push_back(command);

            // If the redo stack exceeds the limit, remove the oldest command
            if (redoStack.size() > maxStackSize) {
                redoStack.pop_front();
            }
        }
    }

    void Redo() {
        if (!redoStack.empty()) {
            auto command = redoStack.back();
            redoStack.pop_back();

            // Execute the do action
            command.doAction();

            // Push the command back to the undo stack
            undoStack.push_back(command);

            // If the undo stack exceeds the limit, remove the oldest command
            if (undoStack.size() > maxStackSize) {
                undoStack.pop_front();
            }
        }
    }

    bool CanUndo() const {
        return !undoStack.empty();
    }

    bool CanRedo() const {
        return !redoStack.empty();
    }

    void Clear() {
        undoStack.clear();
        redoStack.clear();
    }

private:
    std::deque<Command> undoStack;
    std::deque<Command> redoStack;
    static constexpr size_t maxStackSize = 20; // Limit to 20 actions
};
