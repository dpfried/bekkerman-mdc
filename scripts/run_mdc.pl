for($i = 1; $i <= 10; $i++) {
    mkdir "mdc$i";
    chdir "mdc$i";
    system "cp ../*.mdc ../*.ini .";
    system "../../ver3.0/mdc > mdc.log";
    chdir "..";
}
