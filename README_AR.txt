Factory Horror - PSP Prototype
===============================

ده مشروع Homebrew أصلي للـPSP/PPSSPP، مستوحى من أجواء مصانع الألعاب والرعب، وليس نسخة من ملفات Poppy Playtime.

النسخة الحالية:
- حركة اللاعب بالـD-Pad
- شاشة بداية
- Prototype قابل للتطوير
- الهدف التالي: إضافة رسومات 2D ثم 3D، ألغاز، أبواب، وحش، وصوت.

البناء:
1) ثبّت PSPDEV/PSPSDK على كمبيوتر.
2) افتح Terminal داخل مجلد المشروع.
3) نفّذ:
   make

سيظهر EBOOT.PBP.

التثبيت في PPSSPP:
PSP/GAME/FactoryHorror/EBOOT.PBP

مهم:
لا تضع EBOOT.PBP وحده في PSP/GAME؛ ضع مجلد اللعبة كاملًا.

على Android داخل مجلد ألعاب PPSSPP، أنشئ:
PSP/GAME/FactoryHorror/

ثم انسخ EBOOT.PBP إليه، وافتح PPSSPP واختر Homebrew/ألعاب PSP حسب إصدار التطبيق.

الخطوة القادمة:
أقدر أطور هذا المشروع إلى نسخة 2D أو أبدأ نظام 3D منخفض التفاصيل مناسب لقدرات PSP.
