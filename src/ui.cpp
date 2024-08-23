#include <algorithm>
#include <imgui/imgui.h>
#include <optics.h>
#include <ui.h>

static bool StateInputDouble(const char* label, double* v, bool& is_changed)
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
		static const float font_size = ImGui::GetFontSize();

		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
		// create a fullscreen window
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);

		ImGui::Begin("GaussianBeam", NULL, window_flags);

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

		if (ImGui::Button("Add lens", ImVec2(10.0f * font_size, 5.0f * font_size)))
		{
			items.push_back(Lens());
			items_need_sort = true;
			is_changed = true;
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

		ImGui::Dummy(ImVec2(0.0f, font_size));
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Wavelength:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(10.0f * font_size);
		StateInputDouble("nm", &(beam.wavelength_), is_changed);

		if (ImGui::BeginTable("table1", kColumnCount, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV))
		{
			// header row
			ImGui::TableSetupColumn("Optics");
			ImGui::TableSetupColumn("Position(mm)");
			ImGui::TableSetupColumn("Relative position(mm)");
			ImGui::TableSetupColumn("Focus(mm)");
			ImGui::TableSetupColumn("Waist(um)");
			ImGui::TableSetupColumn("Rayleigh range(mm)");
			ImGui::TableHeadersRow();

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, font_size));
			// input beam
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Input Beam");
			ImGui::TableSetColumnIndex(1);
			StateInputDouble("##waist_position", &(beam.waist_pos_), is_changed);
			ImGui::TableSetColumnIndex(4);
			StateInputDouble("##waist", &(beam.waist_), is_changed);
			ImGui::TableSetColumnIndex(5);
			ImGui::Text("%.3f", beam.rayleigh_range());

			// lens
			for (int n = 0; n < items.Size; n++)
			{
				ImGui::PushID(n);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Lens");
				ImGui::TableSetColumnIndex(1);
				if (StateInputDouble("##position", &(items[n].pos_), is_changed))
				{
					items_need_sort = true;
				}
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%.3f", n > 0 ? (items[n].pos_ - items[n - 1].pos_) : (items[0].pos_ - beam.waist_pos_));
				ImGui::TableSetColumnIndex(3);
				StateInputDouble("##focus", &(items[n].f_), is_changed);
				ImGui::TableSetColumnIndex(6);
				if (ImGui::Button("Delete", ImVec2(-FLT_MIN, 0.0f)))
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
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%.3f", items.Size > 0 ? (beam_cal.waist_pos_ - items.back().pos_) : (beam_cal.waist_pos_ - beam.waist_pos_));
			ImGui::TableSetColumnIndex(4);
			ImGui::Text("%.3f", beam_cal.waist_);
			ImGui::TableSetColumnIndex(5);
			ImGui::Text("%.3f", beam_cal.rayleigh_range());

			ImGui::PopStyleVar();
			ImGui::EndTable();
		}

		ImGui::End();
	}
}

