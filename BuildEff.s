
.data
    buffer: .space 64
    swapbuffer: .space 64
    prompt: .asciiz "Enter name of building:\n"
    promptInt: .asciiz "Enter intval of sqft:\n "
    promptFloat: .asciiz "Enter float energy:\n"
    promptEng: .asciiz "This is the energy:\n"
    node_size: .word 72
    done: .asciiz "DONE\n"
    newline: .asciiz "\n"
    space: .asciiz " "

    zeroflt: .float 0.0
    format_string: .asciiz "%.6f\n"

    head: .word 100

    exitmsg: .asciiz "Saw done ending: \n"
    createnodemsg: .asciiz "Creating node: \n"

    equal_msg: .asciiz "Strings are equal.\n"
    not_equal_msg: .asciiz "Strings are not equal.\n"
.text

.globl main

main:
    # creating a stack pointer for return address
    addi $sp, $sp, -4
    sw $ra, 0($sp)

    # THIS IS THE HEAD NODE TOUCH WITH CARE
    li $v0, 9             # syscall 9 (sbrk)
    lw $a0, node_size     # Size of a node (data + next pointer)
    syscall
    # t5 is head node
    move $t5, $v0

inputval:
    li $v0, 4           
    la $a0, prompt      
    syscall

    # Read user input
    li $v0, 8           # syscall 8 (read string)
    la $a0, buffer      # Load the address of the buffer
    li $a1, 64         # Maximum number of characters to read
    syscall

    # storing an unmutated copy to store in node
    move $t2, $a0
    
    # Comparing the input string to "DONE"
    la $a1, done      # Address of the second string

    # Start of string comparison loop
compare_loop:
    lb $t0, 0($a0)    
    lb $t1, 0($a1)    

    beq $t0, $t1, continue_comparison   # continue comparison
    j createfirstnode       # if they're not equal, branch and create node

continue_comparison:
    beq $t0, $zero, end_file      # strings are equal, end file
    addi $a0, $a0, 1  
    addi $a1, $a1, 1  
    j compare_loop


createfirstnode:

    # Initialize node data
    # moving the node created to t3 for perpetuating address
    move $t0, $t5
    move $t3, $t5
    move $t4, $t5 
    move $a1, $t2 # string value
    li $t1, 0

    store_loop:
    lb $t2, 0($a1)      # Load a byte from the source string
    sb $t2, 0($t0)      # Store the byte in the allocated memory
    addi $a1, $a1, 1    # Move to the next byte in source string
    addi $t0, $t0, 1    # Move to the next byte in allocated memory
    addi $t1, $t1, 1    # Increment counter
    blt $t1, 64, store_loop  # Repeat until 64 bytes are stored

    # prompt for int number

    li $v0, 4             
    la $a0, promptInt     
    syscall
    
    li $v0, 5
    syscall
    move $t2, $v0


    # the converted integer sits in register f1
    mtc1 $v0, $f1
    cvt.s.w $f1, $f1


    li $v0, 4           # syscall 4 (print_string)
    la $a0, promptEng      # Load the address of the prompt string
    syscall

    li $v0, 6
    syscall

    # the inputted power sits in register f0 
    beqz $t2, set_eff_f

    # dividing power by sq foot
    div.s $f2, $f0, $f1

    s.s $f2, 64($t3)

    j movereg_f 

    set_eff_f:
    l.s $f4, zeroflt
    s.s $f4, 64($t3)

    movereg_f:
    sw $zero, 68($t3) # - setting next ptr to null



    j createnodes



createnodes:
    # we will now create a node
    # Allocate memory for a node
    li $v0, 9             # syscall 9 (sbrk)
    lw $a0, node_size     # Size of a node (data + next pointer)
    syscall
    
    move $t3, $v0   # address of this node - 

    li $v0, 4           
    la $a0, prompt      
    syscall

    # Read user input
    li $v0, 8           # syscall 8 (read string)
    la $a0, buffer      # Load the address of the buffer
    li $a1, 64         # Maximum number of characters to read
    syscall

    # storing an unmutated copy to store in node
    move $t2, $a0
    
    # Comparing the input string to "DONE"
    la $a1, done      # Address of the second string

    # Start of string comparison loop
    comp_loop:
    lb $t0, 0($a0)    
    lb $t1, 0($a1)    

    beq $t0, $t1, cont_comparison   # continue comparison
    li $a0, 0
    j addvalues       # if they're not equal, branch and add values

    cont_comparison:
    beq $t0, $zero, printlaunch      # strings are equal, no need to make node
    addi $a0, $a0, 1  
    addi $a1, $a1, 1  
    j comp_loop

    printlaunch: 
    move $a0, $t5
    jal insertion_sort
    move $a0, $t5
    jal printnode
    j end_file
    

   
    # Initialize node data
    # moving the node created to t3 for perpetuating address
   
    
    addvalues:

    move $a1, $t2 # string value
    move $t0, $t3 # address of this node
    li $t1, 0

    store_loop1:
    lb $t2, 0($a1)      # Load a byte from the source string
    sb $t2, 0($t0)      # Store the byte in the allocated memory
    addi $a1, $a1, 1    # Move to the next byte in source string
    addi $t0, $t0, 1    # Move to the next byte in allocated memory
    addi $t1, $t1, 1    # Increment counter
    blt $t1, 64, store_loop1  # Repeat until 64 bytes are stored

    # prompt for int number

    li $v0, 4             
    la $a0, promptInt     
    syscall
    
    li $v0, 5
    syscall
    move $t2, $v0


    # the converted integer sits in register f1
    mtc1 $v0, $f1
    cvt.s.w $f1, $f1


    li $v0, 4           # syscall 4 (print_string)
    la $a0, promptEng      # Load the address of the prompt string
    syscall

    li $v0, 6
    syscall


    # the inputted power sits in register f0 
    beqz $t2, set_eff
    # dividing power by sq foot
    div.s $f2, $f0, $f1

    s.s $f2, 64($t3)
    sw $zero, 68($t3) # - setting next ptr to null
    j movereg 

    set_eff:
    l.s $f4, zeroflt
    s.s $f4, 64($t3)
    sw $zero, 68($t3) # - setting next ptr to null

    movereg:
    move $t0, $t5
    move $a1, $t3

    find_last_node:
    lw $t1, 68($t0)     # Load the next pointer of the current node
    beq $t1, $zero, set_next_pointer  # If next pointer is null, set it to the new node
    move $t0, $t1       # Move to the next node
    j find_last_node

    set_next_pointer:
    sw $a1, 68($t0)     # Set the next pointer of the last node to the new node

    j createnodes

    printnode: 
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    move $t4, $a0

    lb $a1, newline


    print_loop:
        move $t3, $t4
    print_loop_internal:
        lb $t2, 0($t3)              # Load a byte from the allocated memory
        beq $t2, $zero, printfloat   # If byte is null terminator, exit loop
        beq $t2, $a1, printfloat
        # Print character
        move $a0, $t2               # Move the character to be printed to $a0
        li $v0, 11                  # Load system call number for printing character
        syscall
        addi $t3, $t3, 1            # Move to the next byte in allocated memory
        j print_loop_internal

    printfloat:
        lb $a0, space
        li $v0, 11                  # Load s
        syscall

        # Print float value
        l.s $f12, 64($t4)   # Load float value from the node
        li $v0, 2           # syscall 2 (print float)
        syscall
    # Print newline
    li $v0, 4           # syscall 4 (print_string)
    la $a0, newline
    syscall

    # Move to the next node
    lw $t4, 68($t4)
    bnez $t4, print_loop  # If not null, continue printing

    lw $ra, 0($sp)
    addi $sp, $sp, 4

    jr $ra

    insertion_sort:
    # Arguments:
    # $a0 - Address of the head node
    addi $sp, $sp, -4
    sw $ra, 0($sp)

    move $t0, $a0       # $t0 points to the head node
    li $s0, 0          # Flag to check if there's a swap
    # let null just be $zero
    li $s1, 0           # lptr
    beqz $t0, return_insertion



    outer_loop:
        move $t1, $t0       # $t1 points to the key node
        li $s0, 0

        inner_loop:
            lw $t3, 68($t1)     # ptr of this node to the next

            beq $t3, $s1, exit_outer # exit condition

            l.s $f11, 64($t1)   # Load float value from the node
            l.s $f12, 64($t3)   # Load float value from the node

            c.eq.s $f11, $f12        # compare f4 and f6; set FCC[0] to the result
            bc1t swap_check

            c.lt.s $f11, $f12        # compare f4 and f6; set FCC[0] to the result
            bc1t swap
           
            j not_swapped

            swap_check:

                move $a0, $t1
                move $a1, $t3

                comp_loop2:
                    lb $t6, 0($a0)    
                    lb $t4, 0($a1)    

                    beqz $t4,swap
                    beqz $t6, not_swapped
                    bgt $t6, $t4, swap   # continue comparison
                    blt $t6, $t4, not_swapped
                # cont_comparison2:
                #    beq $t6, $zero, not_swapped      # strings are equal, no need to make node
                #    beq $t4, $zero, not_swapped
                    addi $a0, $a0, 1  
                    addi $a1, $a1, 1  
                    j comp_loop2
              
            swap:
                li $a0, 0
                move $t6, $t1
                move $t4, $t3
                la $t7, swapbuffer

            store_loop11:
                lb $t2, 0($t6)      # Load a byte from the source string
                sb $t2, 0($t7)      # Store the byte in the allocated memory
                addi $t6, $t6, 1    # Move to the next byte in source string
                addi $t7, $t7, 1    # Move to the next byte in allocated memory
                addi $a0, $a0, 1    # Increment counter
                blt $a0, 64, store_loop11  # Repeat until 64 bytes are stored
                move $t6, $t1
                move $t4, $t3
                li $a0, 0
            store_loop22:
                lb $t2, 0($t4)      # Load a byte from the source string
                sb $t2, 0($t6)      # Store the byte in the allocated memory
                addi $t4, $t4, 1    # Move to the next byte in source string
                addi $t6, $t6, 1    # Move to the next byte in allocated memory
                addi $a0, $a0, 1    # Increment counter
                blt $a0, 64, store_loop22  # Repeat until 64 bytes are stored

                move $t6, $t1
                move $t4, $t3
                li $a0, 0
                la $t7, swapbuffer
            store_loop33:
                lb $t2, 0($t7)      # Load a byte from the source string
                sb $t2, 0($t4)      # Store the byte in the allocated memory
                addi $t4, $t4, 1    # Move to the next byte in source string
                addi $t7, $t7, 1    # Move to the next byte in allocated memory
                addi $a0, $a0, 1    # Increment counter
                blt $a0, 64, store_loop33  # Repeat until 64 bytes are stored

                l.s $f1, 64($t1)   # Load float value from the node
                l.s $f2, 64($t3)   # Load float value from the node

                s.s $f2, 64($t1)
                s.s $f1, 64($t3)
                li $s0, 1         # Set swap flag to 1
                j not_swapped


            not_swapped:

                move $t1, $t3       # Move to the next node
                bnez $t3, inner_loop  # If not null, continue inner loop
 
        move $s1, $t1
        bnez $s0, outer_loop    # If there was a swap, continue outer loop
    
    exit_outer: 
    beqz $s0, return_insertion

    j outer_loop
    
    return_insertion:
    lw $ra, 0($sp)
    addi $sp, $sp, 4

    jr $ra

                

    end_file:
    
    lw $ra, 0($sp)
    addi $sp, $sp, 4

    jr $ra

.end main


    




