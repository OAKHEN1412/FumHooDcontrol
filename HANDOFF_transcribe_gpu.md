# HANDOFF — GPU transcribe `เสียงบันทึกใหม่ 15.m4a`

งานต่อหลัง reboot. session ก่อน setup เสร็จหมด ติดแค่ Smart App Control (SAC).

## เป้าหมาย
Transcribe `sounds/เสียงบันทึกใหม่ 15.m4a` (Thai, 34MB) ด้วย GPU → เขียน `.txt` + `.srt` ข้างไฟล์ต้นทาง.

## เครื่อง
- GPU: **NVIDIA RTX 3050 6GB**, driver 591.86, CUDA 13.1. `nvidia-smi` ใช้ได้.
- Python 3.12 (Windows native): `C:\Users\Tech3\AppData\Local\Programs\Python\Python312`

## ของที่ลงแล้ว (session ก่อน — ใช้ได้หมด)
- `torch 2.6.0+cu124` — import ผ่าน SAC, `torch.cuda.is_available()` = True, เห็น RTX 3050
- `faster-whisper` + `ctranslate2` (pip)
- `openai-whisper` + `tiktoken` (pip)
- `ffmpeg 8.1.1` (winget Gyan.FFmpeg) — exe: `C:\Users\Tech3\AppData\Local\Microsoft\WinGet\Packages\Gyan.FFmpeg_Microsoft.Winget.Source_8wekyb3d8bbwe\ffmpeg-8.1.1-full_build\bin\ffmpeg.exe` (PATH เพิ่งแก้ ต้อง shell ใหม่หลัง reboot ถึงเห็น)

## ปัญหาที่เจอ + ทำไม reboot
**Smart App Control (SAC) = ON** บล็อก unsigned native DLL ทุกตัวของ whisper:

| native ext | SAC |
|-----------|-----|
| torch | ✅ ผ่าน |
| PyAV (`av`) | ❌ บล็อก |
| tiktoken (`_tiktoken`) | ❌ บล็อก |
| ctranslate2 | ❌ บล็อก |

user เลือก **ปิด SAC** (ทางเดียว, ปิดแล้วเปิดกลับต้อง reset Windows). เปิดหน้า `windowsdefender://smartappcontrol` ให้แล้ว user toggle Off + reboot.

## ทำต่อหลัง reboot (ลำดับสำคัญ)

1. **ยืนยัน SAC ปิดจริง** — ต้องได้ `0` (ถ้าได้ `1` = ยังไม่ปิด/ยังไม่ reboot, หยุด แจ้ง user):
   ```powershell
   (Get-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Control\CI\Policy').VerifiedAndReputablePolicyState
   ```

2. **รัน GPU transcribe** — script พร้อมแล้วที่ `C:\Users\Tech3\.claude\skills\transcribe-audio\transcribe_fw.py` (faster-whisper, decode เสียงด้วย ffmpeg → numpy, stub `av`, CUDA float16, fallback ครบ). รัน background:
   ```
   cd "C:/Users/Tech3/OneDrive/เอกสาร/Arduino/fumhood"
   python -X utf8 "C:/Users/Tech3/.claude/skills/transcribe-audio/transcribe_fw.py" "sounds/เสียงบันทึกใหม่ 15.m4a" --model large-v3 --lang th
   ```
   - large-v3 โหลด weights ~3GB ครั้งแรก (cache `~/.cache/huggingface`)
   - VRAM: 6GB total, ~785MB ใช้อยู่ → large-v3 float16 ~4.7GB อาจตึง ถ้า OOM ลด `--model medium`
   - output progress ต่อ segment ใน output file

3. **เสร็จแล้ว** — `.txt` + `.srt` อยู่ใน `sounds/` ข้าง .m4a. offer อ่านกลับให้ user.

## หมายเหตุ
- SAC ปิดแล้ว → `transcribe_fw.py` (stub av) ยังใช้ได้ปกติ; หรือจะใช้ skill เดิม `transcribe.py` (CPU) / เขียน faster-whisper ตรงๆ ก็ได้เพราะ ctranslate2/av โหลดผ่านแล้ว. แต่ `transcribe_fw.py` = GPU robust สุด ใช้ตัวนี้.
- ถ้า user เปลี่ยนใจไม่ปิด SAC → ทางสำรองคือ WSL2 + Ubuntu (Linux ไม่มี SAC).
