#pragma once

#include "pch.hpp"
#include "UIComponent.hpp"

/** Lua Console
 * Log all program messages.
 * Log all Lua output
 * Accept Lua code
 * Print back out entered Lua code
 * Log Lua error messages.
 * User can click on lua code to paste it back into the entry box
 *
 */


namespace ui {
    // Container for console log messages and destination for spdlog.
    // The idea is that spdlog can insert into the container even before the console UI is available.
    // Store text colors, that kind of related information.
    // ? On colors, we can cache unique colors in a vector<Color>, then store just size_t color_index
    class ConsoleStore {
        // vector<string, packing_allocator> is the most efficient an array of strings can be.
        std::vector<std::string> lines {};
    };

    class UIConsole final : public UIComponent {
        /* Check LINE against what HISTCONTROL says to do.  Returns 1 if the line
           should be saved; 0 if it should be discarded. */
        //int check_history_control (const char *line);

        /* Add LINE to the history list, handling possibly multi-line compound
           commands.  We note whether or not we save the first line of each command
           (which is usually the entire command and history entry), and don't add
           the second and subsequent lines of a multi-line compound command if we
           didn't save the first line.  We don't usually save shell comment lines in
           compound commands in the history, because they could have the effect of
           commenting out the rest of the command when the entire command is saved as
           a single history entry (when COMMAND_ORIENTED_HISTORY is enabled).  If
           LITERAL_HISTORY is set, we're saving lines in the history with embedded
           newlines, so it's OK to save comment lines.  If we're collecting the body
           of a here-document, we should act as if literal_history is enabled, because
           we want to save the entire contents of the here-document as it was
           entered.  We also make sure to save multiple-line quoted strings or other
           constructs. */
        //void maybe_add_history (const char *line);

        /* Just check LINE against HISTCONTROL and HISTIGNORE and add it to the
           history if it's OK.  Used by `history -s' as well as maybe_add_history().
           Returns 1 if the line was saved in the history, 0 otherwise. */
        //int check_add_history (const char *line, int force)

        /* Add a line to the history list.
           The variable COMMAND_ORIENTED_HISTORY controls the style of history
           remembering;  when non-zero, and LINE is not the first line of a
           complete parser construct, append LINE to the last history line instead
           of adding it as a new line. */
        //void bash_add_history (const char *line)

        /* Return 1 if we should not put LINE into the history according to the
           patterns in HISTIGNORE. */
        //int history_should_ignore (const char *line)

        int TextEditCallback(ImGuiInputTextCallbackData *data);
        void ClearLog();
        void AddLog(const char *fmt, ...);
        void ExecCommand(const char *command_line);
    public:
        UIConsole();
        ~UIConsole() override;
        void draw(bool *p_open) override;
    private:
        char InputBuf[256];
        ImVector<char *> Items;
        ImVector<const char *> Commands;
        ImVector<char *> History;
        int HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
        ImGuiTextFilter Filter;
        bool AutoScroll;
        bool ScrollToBottom;
    };
}
