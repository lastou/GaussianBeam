#include <algorithm>
#include <imgui/imgui.h>
#include <optics.h>
#include <ui.h>

static bool CellInputDouble(const char* label, double* v, bool& is_changed)
{
	ImGui::InputDouble(label, v, 1.0, 10.0, "%.3f");
	bool state = ImGui::IsItemDeactivatedAfterEdit();
	is_changed = is_changed || state;
	return state;
}

namespace window_ui
{

	void RenderUI()
	{
		static Beam beam(1064, 50, 0);
		static ImVector<Lens> items;

		constexpr int kColumnCount = 7;
		static Beam beam_cal(beam);
		static bool is_initialized = false;
		static bool is_changed = true;
		static bool items_need_sort = false;
		static bool items_need_delete = false;
		static int delete_n = 0;

		ImGui::Begin("Lens");

		if (!is_initialized)
		{
			items.resize(1, Lens());
			is_initialized = true;
		}

		if (items_need_delete)
		{
			items.erase(items.Data + delete_n);
			items_need_delete = false;
		}

		if (items_need_sort && items.Size > 1)
		{
			std::sort(items.begin(), items.end(), [](Lens a, Lens b) {return a.pos_ < b.pos_;});
			items_need_sort = false;
		}

		if (is_changed)
		{
			beam_cal.Update(beam);
			for (int n = 0;n < items.Size;n++)
			{
				items[n].Transform(beam_cal);
			}
			is_changed = false;
		}

		if (ImGui::Button("Add lens"))
		{
			items.push_back(Lens());
			items_need_sort = true;
			is_changed = true;
		}

		ImGui::Text("Wavelength:");
		ImGui::SameLine();
		CellInputDouble("nm", &(beam.wavelength_), is_changed);

		if (ImGui::BeginTable("table1", kColumnCount))
		{
			// header row
			ImGui::TableSetupColumn("Optics");
			ImGui::TableSetupColumn("Position(mm)");
			ImGui::TableSetupColumn("Relative position(mm)");
			ImGui::TableSetupColumn("Focus(mm)");
			ImGui::TableSetupColumn("Waist(um)");
			ImGui::TableSetupColumn("Rayleigh range(mm)");
			ImGui::TableHeadersRow();

			// input beam
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Input Beam");
			ImGui::TableSetColumnIndex(1);
			CellInputDouble("##waist_position", &(beam.waist_pos_), is_changed);
			ImGui::TableSetColumnIndex(4);
			CellInputDouble("##waist", &(beam.waist_), is_changed);
			ImGui::TableSetColumnIndex(5);
			ImGui::Text("%.3f", beam.rayleigh_range());

			// lens
			for (int n = 0; n < items.Size; n++)
			{
				ImGui::PushID(n);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Lens");
				ImGui::TableSetColumnIndex(1);
				if (CellInputDouble("##position", &(items[n].pos_), is_changed))
				{
					items_need_sort = true;
				}
				ImGui::TableSetColumnIndex(3);
				CellInputDouble("##focus", &(items[n].f_), is_changed);
				ImGui::TableSetColumnIndex(6);
				if (ImGui::Button("Delete"))
				{
					items_need_delete = true;
					delete_n = n;
					is_changed = true;
				}
				ImGui::PopID();
			}

			// output beam
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Output Beam");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.3f", beam_cal.waist_pos_);
			ImGui::TableSetColumnIndex(4);
			ImGui::Text("%.3f", beam_cal.waist_);
			ImGui::TableSetColumnIndex(5);
			ImGui::Text("%.3f", beam_cal.rayleigh_range());

			ImGui::EndTable();
		}


		ImGui::End();
		// window option, move, resize, collapse
		// font
		// icon
	}
}

