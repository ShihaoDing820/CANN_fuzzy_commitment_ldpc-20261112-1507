# Fuzzy Commitment QC-LDPC CANN Prototype

This is a correctness-oriented CANN 9.0.0 CPU twin-debug prototype. It implements:

- a fixed `(3, 6)` regular QC-LDPC graph (`N=96`, rank `46`, dimension `K=50`);
- enrollment helper data `delta = witness XOR codeword`;
- fused helper XOR, LLR initialization, normalized Min-Sum decoding, and syndrome checking in an Ascend C kernel;
- SHA-256 commitment verification on the host with a constant-time comparison;
- ten deterministic end-to-end cases: eight legitimate noisy witnesses and two impostors;
- a host reference decoder used to compare every CANN output bit and decoder status.

Run:

```bash
/home/dsh/cann-dev/fuzzy-commitment/run-tests.sh
```

The algorithm organization was informed by these open-source projects, without copying their CUDA/PyTorch source:

- `robertwgh/cuLDPC` (Apache-2.0): CUDA QC-LDPC Min-Sum/SPA decoder.
- `NVlabs/sionna-rk` (Apache-2.0): CUDA LDPC decoder using minimum/second-minimum check-node updates.
- `Lin-X-H/WCFC`: PyTorch fuzzy commitment flow using LDPC encode, biometric XOR, decode, and SHA-256 verification.

This short code is intended to prove that the protocol and decoder can compile and run through CANN CPU twin debug. It is not a production biometric security system and is not performance-optimized for a real Ascend NPU.
