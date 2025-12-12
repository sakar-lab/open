#!/usr/bin/env bash

# Completion function for open
_auto_completion() {
    # Current word being completed
    local cur="${COMP_WORDS[COMP_CWORD]}"

    # List of subcommands/words to complete
    local opts
        opts="$(open --commands 2>/dev/null)" || return 0

    # Fills COMPREPLY array safely from compgen output
    mapfile -t COMPREPLY < <(compgen -W "$opts" -- "$cur")
}

# Attach this function to the command name
complete -F _auto_completion open