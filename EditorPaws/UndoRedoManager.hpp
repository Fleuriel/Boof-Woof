#include <vector>
#include <functional>
#include <stack>

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
        undoStack.push({ doAction, undoAction });

        // Clear redo stack when a new command is executed
        while (!redoStack.empty()) {
            redoStack.pop();
        }
    }

    void Undo() {
        if (!undoStack.empty()) {
            auto command = undoStack.top();
            undoStack.pop();

            // Execute the undo action
            command.undoAction();

            // Push the command to the redo stack
            redoStack.push(command);
        }
    }

    void Redo() {
        if (!redoStack.empty()) {
            auto command = redoStack.top();
            redoStack.pop();

            // Execute the do action
            command.doAction();

            // Push the command back to the undo stack
            undoStack.push(command);
        }
    }

    bool CanUndo() const {
        return !undoStack.empty();
    }

    bool CanRedo() const {
        return !redoStack.empty();
    }

private:
    std::stack<Command> undoStack;
    std::stack<Command> redoStack;
};
