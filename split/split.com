h��hR	h7�h�	h9��  �� �� ���΁Ƃ ��P �Q �=  �N  �< �; �6N�Ƃ ��� t1��	t,���-��"t�P�>=����=��*Q�Q��>P ��>Qu�>=����=�� �P �>=ƅ� �><tq�=  h�h��>R�sh�h$��>RtBh�h'��>Rt2�>; t
�>;t���;hR	h��� �;h�	h���t�r�<�k�< �6=N���<bt)<kt0<mt7h���>S ���=  f�3f�/�0f�3   �f�3   �f�3   � Ƅ� ��Q ���NI����><�YhR	h7���>Rt�=�R	� �!�P��  �?�K  f�C    �>K tf�/f9CuR�>�A�!�K�>K��� hP
h�	�h��KHP���� hP
h���<�P
�  �!�� �Af�C    f�/f+Cf=   vf�   �G�?�?��
�G�!�I�@�A��
�I�!f�Cf�    �If�f�C��D�?�!< �<��>�A�!�>�?�!� hC�:h��<� h��3h��-�}h��%h���ohC��6=Ƅ�$h�� hK�h��� �LhC�� he�� h��� �8hC�� hR	� h��� �$hC�� hP
� h�� �>�?�!�h�� � � ]^f1��Sf1ۊ�� t��0r��9wfk�
f��0f�F���S U�]^_�R��< t�� tFG8�t�8�t�R U�]^_��FG< u�U�]^_�G< u�O��FG< u�U�]^��� t��!F��U�]X_����aa��eU�]Z�	�!U�]Z�	�!���!�
�!U�Split v0.1 - Program to split large file
(c) Jan Frisse 2002. Made in pure assembler FASM. Published under GPL licence.$split : to few no option arguments$Try "split --help" for more information.$Ussage : split [OPTION] [INPUT] [PREFIX]
Output fixed-size pieces of INPUT to PREFIXaa, PREFIXab, ...
Default PREFIX is "x". If input is "-", read standart input.
Options : 
  -b, --bytes SIZE	Put SIZE bytes per output file.

Default SIZE is 1457500
SIZE may have a multiplier suffix: b for 512, k for 1K, m for 1 Meg.

Report bugs to <janf@email.cz>$split: $: invalid number of bytes$option "--bytes" or "-b" requires an argument$split: too many arguments$: No such file or directory$: Can't open file for writing$Too much parts of splited file. Maximal is 676 parts.$--help -b --bytes \=    - x 